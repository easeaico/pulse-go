/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.testing;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import com.kitware.pulse.utilities.FileUtils;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.RunConfiguration;
import com.kitware.pulse.utilities.JNIBridge;

public class Rebase
{
  public static void main(String[] args)
  {
    String hash = null;
    if(args.length != 0)
      hash = args[0];
    JNIBridge.initialize();
    rebase(hash);
  }
  
  public static void rebase(String hash)
  {
    RunConfiguration cfg = new RunConfiguration();
    String toDir = "./test_results/rebase/";
    List<String> configFiles = new ArrayList<>();
    try
    {
      File file = new File(cfg.getTestConfigDirectory()+"/Rebase.config");
      FileReader fileReader = new FileReader(file);
      BufferedReader bufferedReader = new BufferedReader(fileReader);
      String line;
      while ((line = bufferedReader.readLine()) != null) 
      {
        line = line.trim();
        if(line.indexOf("#")>-1)
          continue;
        if(line.indexOf(".config")==-1)
            continue;
        configFiles.add(line);
      }
      fileReader.close();
    }
    catch(IOException ex)
    {
      Log.error("Unable to open Rebase.config, check your run.config file test_config_dir");
      return;
    }
    
    // Replace back slashes with forward slashes
    toDir = toDir.replaceAll("\\\\", "/");
    toDir += new SimpleDateFormat("yyyy-MM-dd").format(new Date());
    if(hash==null || hash.isEmpty())
      hash = "nogithash";
    toDir += " - "+hash+"/";

    Log.info("Rebasing to "+toDir);

    // Make sure from/to dir ends with a slash
    if(!toDir.endsWith("/"))
      toDir = toDir + "/";
    try
    {
      FileUtils.delete(toDir);
      FileUtils.createDirectory(toDir);
    }
    catch(Exception ex)
    {
      Log.error("Could not create to Directory : "+toDir);
      Log.error(ex.getMessage());
      return;
    }

    String log_file;
    String result_path;
    List<String> result_files = new ArrayList<>();

    for(String configFile : configFiles)
    {
      Log.info("Rebasing config "+configFile);
      SETestConfiguration config = new SETestConfiguration();
      config.processConfigFile(new File(cfg.getTestConfigDirectory()+"/"+configFile), cfg);

      for(SETestJob job : config.jobs)
      {
        if(job.computedFiles.size()<=0)
        {
          Log.error("A job has no expected result files!?!?");
          continue;
        }
        if(!job.plottableResults)
          continue;// Nothing to plot/compare, nothing to zip  
        // We assume all json will have something to zip...
        
        log_file = "";
        result_files.clear();
        result_path = job.computedFiles.get(0);
        if(!new File(result_path).exists())
        {
          Log.error("Unable to find file to rebase for "+job.name+" at path "+result_path);
          continue;
        }
        String basename = result_path.replaceAll("Results.csv", "");
        
        // If there is no baselineDirectory, then this must be an assessment...
        if(job.name.endsWith(".json"))
        {
          if(job.computedFiles.size()>1)
          {
            Log.error("A scenario file has more than one result file!?!?");
            continue;
          }
          result_files.add(result_path);
          // See if the log file is named EXACTLY like the csv file
          log_file = result_path.replaceAll(SETestConfiguration.ext, ".log");
          if (new File(log_file).exists())
            result_files.add(log_file);
          else
          {
            // If not, take off 'Results'
            log_file = result_files.get(0).replaceAll("Results"+SETestConfiguration.ext, ".log");
            if (new File(log_file).exists())
              result_files.add(log_file);
          }
          // Are there any assessment json's here?
          int split = basename.lastIndexOf('/');
          List<String> file_list = FileUtils.findFiles(basename.substring(0, split), basename.substring(split+1), false);
          for(String file : file_list)
          {
            if(file.indexOf('@') >= 0)
              result_files.add(file.substring(file.indexOf('.')).replaceAll("\\\\", "/"));
          }
          
          if (job.isValidation)
            result_files.add(result_path.replace(".csv", "-Segments.json"));
          Log.info("Creating zip for "+result_files.get(0));
        }      
        else
        {
          // Its a unit test, only zip up the results
          result_files.addAll(job.computedFiles);
        }

        String prepend = result_path.substring(result_path.indexOf("test_results/")+13,result_path.lastIndexOf("."));
        int ridx = prepend.indexOf("Results");
        if(ridx>-1)
          prepend = prepend.substring(0,ridx);
        String newFile = toDir + prepend + ".zip";

        Log.info("Creating new file "+newFile);
        String rPath = "";
        for(String rDir : newFile.split("/"))
        {
          if(rDir.endsWith(".zip"))
          {
            break;
          }
          rPath += rDir + "/";
        }
        try
        {
          Log.info("Creating "+rPath);
          FileUtils.createDirectory(rPath);
        } 
        catch (IOException ex)
        {
          Log.error("Could not create directory : "+rPath);
          Log.error(ex.getMessage());
        }
        File oldZip = new File(newFile);
        if(oldZip.exists())
          oldZip.delete();
        String[] files = new String[result_files.size()];
        files = result_files.toArray(files);
        FileUtils.zipFiles(files, newFile,prepend+".zip");
        // Test our zip is what we expect it to be
        //FileUtils.unzipToDirectory(newFile, newFile.substring(0,newFile.lastIndexOf("/")));
      }
    }
  }

}
