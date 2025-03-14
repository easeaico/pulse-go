package com.kitware.pulse.cdm.testing;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import com.google.protobuf.InvalidProtocolBufferException;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.scenario.SEScenario;
import com.kitware.pulse.cdm.scenario.SEScenarioExec;
import com.kitware.pulse.engine.bind.Enums.eModelType;
import com.kitware.pulse.utilities.FileUtils;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.PythonUtils;
import com.kitware.pulse.utilities.RunConfiguration;
import com.kitware.pulse.utilities.csv.plots.CSVComparePlotter.PlotType;

public class SETestConfiguration 
{
  public static final String ext=".csv";
  public static String sce_ext=".json";
  protected String testName;
  protected String reportName;
  protected int    numThreads=0;
  protected double percentDifference=2.0;

  protected boolean useStates=false;
  protected SEScenarioExec sceExec=new SEScenarioExec();
  protected String patientFiles;
  protected boolean executeJobs=true;
  protected boolean plotResults=true;
  protected boolean pythonComparison=true;
  protected PythonUtils python = null;

  protected Map<String,String> macros = new HashMap<>();
  protected Map<String,Class<? extends SETestDriver.Executor>> executors = new HashMap<>();
  protected List<SETestJob> jobs = new ArrayList<>();  

  protected Map<SETestJob,String> job2groups = new HashMap<>();
  protected Map<String,List<String>> groups = new LinkedHashMap<>();

  public void processConfigFile(File configFile, RunConfiguration cfg)
  {
    String scenarioDir = cfg.getScenarioDirectory();
    String baselineDir = cfg.getVerificationDirectory();
    this.testName = configFile.getName();
    this.testName = this.testName.substring(0,this.testName.lastIndexOf('.'));

    String key;
    String value;

    this.percentDifference = 2.0;

    this.reportName = "TestDriver Report";

    this.executors.clear();
    this.jobs.clear();

    String currentGroup = this.testName;

    // Parse the config file
    // TODO, make sure all \ are converted to /
    try
    {
      BufferedReader br = new BufferedReader(new FileReader(configFile));
      String line;
      while ((line = br.readLine()) != null)
      {
        if (line.length() == 0 || line.startsWith("#"))
          continue;
        if (line.endsWith("\\"))
        {
          String extLine;
          while ((extLine = br.readLine()) != null)
          {
            line += extLine.trim();
            if (extLine.endsWith("\\"))
              continue;
            break;
          }
          line=line.replaceAll("\\\\", "");
        }
        if(line.startsWith("@group"))
        {
          currentGroup = line.substring(6).trim();
          if(currentGroup.isEmpty())
            currentGroup = this.testName;
          continue;
        }
        if(line.indexOf('=') == -1)
          continue;

        line = line.trim();
        key = line.substring(0, line.indexOf('='));
        value = line.substring(line.indexOf("=") + 1);

        if(key.equalsIgnoreCase("ReportName"))
        { this.reportName = value; continue; }

        if(key.equalsIgnoreCase("PercentDifference"))
        { this.percentDifference = Double.parseDouble(value); continue; }
        if(key.equalsIgnoreCase("Threads"))
        { this.numThreads = Integer.parseInt(value); continue; }
        if(key.equalsIgnoreCase("Patients"))
        { this.patientFiles = value; continue; }
        if(key.equalsIgnoreCase("UseStates"))
        {
          if(value.equalsIgnoreCase("true"))
            this.useStates=true; 
          continue; 
        }
        if(key.equalsIgnoreCase("AutoSerialization"))
        {
          // Parse the value
          String[] values = value.split(",");
          this.sceExec.setLogToConsole(eSwitch.On);
          this.sceExec.setOutputRootDirectory("");
          this.sceExec.setAutoSerializePeriod_s(Double.parseDouble(values[0]));
          this.sceExec.setTimeStampSerializedStates(eSwitch.valueOf(values[1]));
          this.sceExec.setAutoSerializeAfterActions(eSwitch.valueOf(values[2]));
          continue; 
        }
        if(key.equalsIgnoreCase("ExecuteTests"))
        {
          if(value.equalsIgnoreCase("false"))
            executeJobs=false; 
          continue; 
        }
        if(key.equalsIgnoreCase("PlotResults"))
        {
          if(value.equalsIgnoreCase("false"))
            plotResults=false; 
          continue; 
        }
        if(key.equalsIgnoreCase("CompareWith"))
        {
          if(value.startsWith("Java"))
            pythonComparison=false;
          else
            python = new PythonUtils();
          continue; 
        }
        if(key.equalsIgnoreCase("Executor"))
        {
          Class<? extends SETestDriver.Executor> clazz = null;
          try
          {
            clazz = Class.forName(value).asSubclass(SETestDriver.Executor.class);
            this.executors.put(clazz.getSimpleName(), clazz);
          } 
          catch(Exception e){Log.error("Could not find Executor "+value);}
          continue;
        }

        if(key.startsWith("Macro"))
        {
          macros.put(key.substring(6), value);
          continue;
        }

        for(String macro : macros.keySet())
        {
          String buffer = " "+value+" ";
          if(buffer.indexOf(" "+macro+" ")>-1)
            value = buffer.replaceAll(" "+macro+" ", " "+macros.get(macro)+" ");
        }

        SETestJob job = new SETestJob();
        job.useState = this.useStates;
        // Copy shared exec options
        job.execOpts.copy(this.sceExec);
        // More exec options
        if(!executeJobs)
          job.skipExecution = true;
        if(!plotResults)
          job.skipPlot = true;
        job2groups.put(job, currentGroup);
        if (key.charAt(0) == '*')
        {
          job.knownFailure = true;
          job.state = SETestJob.State.Complete;
          key = key.substring(1);
        }
        if (key.charAt(0) == '-')
        {
          job.skipExecution = true;
          job.skipPlot = true;
          key = key.substring(1);
        }
        this.jobs.add(job);
        job.name = key.trim();
        job.percentDifference = this.percentDifference;
        if(this.python != null && this.python.isActive())
          job.python = this.python;

        String[] directives = value.trim().split(" ");
        for(String directive : directives)
        {
          if(directive.indexOf("=")==-1)
          {
            if(executors.containsKey(directive))
            {
              try{job.executor = executors.get(directive).getConstructor().newInstance();}
              catch (Exception e)
              {
                job.knownFailure = true;
                Log.error("Could not make a new "+executors.get(directive).getName()+" executor");
              }
              continue;
            }
            if(directive.equalsIgnoreCase("Validation")) 
            { job.isValidation = true; job.scenarioDirectory = "./validation/scenarios/"; continue; }
            if(directive.equalsIgnoreCase("NoCompare")) 
            { job.plottableResults = false; continue; }
            if(directive.equalsIgnoreCase("FastPlot")) 
            { job.plottableResults = true; job.plotType=PlotType.FastPlot; continue; }
            if(directive.equalsIgnoreCase("FullPlot"))
            { job.plottableResults = true; job.plotType=PlotType.FullPlot; continue; }
            if(directive.equalsIgnoreCase("FullPlotErrors"))
            { job.plottableResults = true; job.plotType=PlotType.FullPlotErrors; continue; }
            if(directive.equalsIgnoreCase("FastPlotErrors"))
            { job.plottableResults = true; job.plotType=PlotType.FastPlotErrors; continue; }
            if(directive.equalsIgnoreCase("MemoryFastPlot"))
            { job.plottableResults = true; job.plotType=PlotType.MemoryFastPlot; continue; }
            try {
              job.execOpts.setModelType(eModelType.valueOf(directive));
              continue;
            }catch(IllegalArgumentException e)
            { job.execOpts.setModelType(eModelType.UNRECOGNIZED); }
            
            Log.warn("Unknown directive : " + directive);
          }
          else
          {
            key = directive.substring(0, directive.indexOf('='));
            value = directive.substring(directive.indexOf("=") + 1);
            if(key.equalsIgnoreCase("Baseline"))
            {
              if(value.equalsIgnoreCase("scenarios/"))
              {
                if (job.isValidation)
                  job.scenarioDirectory = "./validation/scenarios/";
                else
                  job.scenarioDirectory = scenarioDir+"/";
              }
              job.baselineDirectory = baselineDir+"/"+value;
              continue;
            }
            if(key.equalsIgnoreCase("Scenario"))
            {job.scenarioDirectory = value; continue;}
            else if(key.equalsIgnoreCase("Computed"))
            {job.computedDirectory = value; continue;}
            if(key.equalsIgnoreCase("Results"))
            {job.resultsFiles = value; continue;}   
          }
        }
        if(job.resultsFiles!=null && !job.resultsFiles.isEmpty())
        {
          for(String result : job.resultsFiles.split(","))
          {
            job.baselineFiles.add(job.baselineDirectory+"/"+result+ext);
            job.computedFiles.add(job.computedDirectory+"/"+result+ext);
          }
        }
        else
        {
          if(job.executor==null)
          {
            job.computedFiles.add(job.computedDirectory+"/"+job.name);
          }
          else if(job.executor.getClass().getName().indexOf("Scenario")!=-1)//Is a Scenario File test
          {
            deriveScenarioResultNames(job, job.name);
          }
          else
          {
            job.baselineFiles.add(job.baselineDirectory+"/"+job.name+ext);
            job.computedFiles.add(job.computedDirectory+"/"+job.name+ext);
          }
        }
      }

      if(this.patientFiles!=null)
      {
        // Need to copy all the jobs and specify a particular patient file
        List<String> patientFileNames;

        if(patientFiles.equalsIgnoreCase("all"))
          // By default, let's not do all patient files recursively
          // Validation spreadsheets will generate patient files in directories here
          patientFileNames = FileUtils.findFiles("./patients", sce_ext, false);
        else if(patientFiles.equalsIgnoreCase("all-recursive"))
          patientFileNames = FileUtils.findFiles("./patients", sce_ext, true);
        else
        {
          String[] patientFiles = this.patientFiles.split(",");
          patientFileNames = new ArrayList<>();
          for(String patientFile : patientFiles)
          {
            patientFile = patientFile.trim();
            if(!patientFile.endsWith(sce_ext))
              patientFile += sce_ext;
            patientFileNames.add(patientFile);
          }
        }

        List<SETestJob> oldJobs = new ArrayList<>(jobs);
        jobs.clear();
        SETestJob copy;

        for(String pFileName : patientFileNames)
        {
          String[] split = pFileName.split("[/\\\\]");
          pFileName = split[split.length-1];

          // Look for scenario jobs and make copies for each of the different patient files to use
          for(SETestJob job : oldJobs)
          {
            if(job.executor==null || job.executor.getClass().getName().indexOf("Scenario")==-1)
              continue;

            copy = job.clone();
            copy.patientFile = pFileName;
            
            String baseName = copy.name;
            deriveScenarioResultNames(copy, baseName.replaceAll(sce_ext, "-"+pFileName));
            
            jobs.add(copy);
          }
        }
        // Now look for the non scenario jobs, so just add those back as is
        for(SETestJob job : oldJobs)
        {
          if(job.executor==null || job.executor.getClass().getName().indexOf("Scenario")==-1)
            jobs.add(job);
        }

      }
      br.close();
    }
    catch (IOException ex)
    {
      Log.error("Ouch");
      Log.error(ex.getMessage());
    }
  }
  
  public void deleteExistingResults()
  {
  // Let's clean out everything we are about to run
    for(SETestJob job : jobs)
    {
      deleteTestResults(job.computedDirectory+"/"+job.name);
      for(String resultFile : job.computedFiles)
      {
        deleteTestResults(resultFile);
      }
    }
  }

  protected void deriveScenarioResultNames(SETestJob job, String baseName)
  {

    job.baselineFiles.clear();
    job.computedFiles.clear();

    String baseline;
    String patientBaseline;
    String output;
    String patientOutput;
    
    String[] dirs = baseName.substring(0, baseName.indexOf(sce_ext)).split("[/\\\\]");
    
    baseline = job.baselineDirectory;
    for(int i=0; i<dirs.length-1; i++)
      baseline+="/"+dirs[i];

    output = job.computedDirectory;
    for(int i=0; i<dirs.length; i++)
      output+="/"+dirs[i];
    
    baseline+="/"+dirs[dirs.length-1];
    patientBaseline = baseline;
    baseline+="Results"+ext;
    patientOutput = output;
    output+="Results"+ext;
    //example : ./Scenarios/Validation/Patient-ValidationResults.csv
    if(new File(baseline).exists() == false && patientFiles == null)
    {
      try
      {
        // The baseline does not exist
        // but the baseline file could be for a specific patient
        // so let's see if a baseline with patient name exists
        String patientFile;
        SEScenario sce = new SEScenario();
        sce.readFile(job.scenarioDirectory+job.name);
        if(sce.hasPatientConfiguration() && sce.getPatientConfiguration().hasPatientFile())
        {
          patientFile = sce.getPatientConfiguration().getPatientFile();
          patientFile = patientFile.trim();
          int start = patientFile.lastIndexOf("/");
          patientFile = patientFile.substring(start==-1?0:start+1,patientFile.indexOf(sce_ext));
          patientBaseline+="-"+patientFile+"Results"+ext;
          if(new File(patientBaseline).exists())
          {
            baseline = patientBaseline;
            output = patientOutput+"-"+patientFile+"Results"+ext;
            job.patientFile = patientFile+sce_ext;
          }
        }
      }
      catch (InvalidProtocolBufferException e){} // Carry on, we'll just plot the computed
    }
    
    job.baselineFiles.add(baseline);
    job.computedFiles.add(output);
  }

  protected void deleteTestResults(String hint)
  {// Try and delete all files related to this test
    // There are lots of various naming conventions, and this should support all of them

    // Get rid of any extensions
    hint = hint.replaceAll(sce_ext, "");
    hint = hint.replaceAll(ext, "");

    File file;

    if(executeJobs)
    {
      file = new File(hint+".log");
      FileUtils.delete(file);

      file = new File(hint+"Test.log");
      FileUtils.delete(file);

      file = new File(hint+ext);
      FileUtils.delete(file);

      file = new File(hint+sce_ext);
      FileUtils.delete(file);

      file = new File(hint+"Test.json");
      FileUtils.delete(file);

      file = new File(hint+"Report.json");
      FileUtils.delete(file);

      file = new File(hint+"Results"+ext);
      FileUtils.delete(file);
    }

    if(plotResults)
    {
      file = new File(hint+"/");
      FileUtils.delete(file);

      file = new File(hint+"Results");
      FileUtils.delete(file);
    }
  }
}
