package com.kitware.pulse.cdm.testing;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.List;

import com.kitware.pulse.utilities.JNIBridge;

/** Going to gather up all the html files in the test_results directory and make one big html report */
public class SEReportResults 
{
  public static void main(String[] args)
  {
    JNIBridge.initialize();
    try
    {
      // Here are the files we are looking for
      // And the order we want them in full report
      List<String> files = new ArrayList<>();
      files.add("CDMUnitTests.html");
      files.add("EngineUnitTests.html");
      files.add("ScenarioVerification.html");
      files.add("DrugPKVerification.html");
      files.add("PatientVerification.html");
      files.add("PatientValidation.html");
      
      String line;
      int start,end;
      boolean copy=false;
      StringBuilder buffer = new StringBuilder();
      buffer.append("<html>");
      buffer.append("<head><title>Full Pulse Test Report</title></head>");
      buffer.append("<body><br>");

      for(String result : files)
      {
        
        if(!new File("./test_results/"+result).exists())
        {
          buffer.append("<h1> "+result+" was not run</h1><br>");
          continue;
        }
        
        BufferedReader br = new BufferedReader(new FileReader("./test_results/"+result));
        while ((line = br.readLine()) != null)
        {
          start = -1;
          end   = -1;
          start = line.indexOf("<body>");
          end = line.indexOf("</body>");
          
          // Is everything on one line?
          if(start>-1 && end>-1)
          {
            buffer.append(line.substring(start+6, end));
            break;
          }
          
          // Is everything on multiple lines?
          if(start>-1)
          {
            buffer.append(line.substring(start+6));
            copy=true;
            continue;
          }
          if(end>-1)
          {
            buffer.append(line.substring(0,end));
            br.close();
            break;
          }
          if(!copy)
            continue;
          buffer.append(line);
        }
      }
      
      buffer.append("</body>");
      buffer.append("</html>");

      BufferedWriter out = new BufferedWriter(new FileWriter("./test_results/PulseTestReport.html"));
      out.write(buffer.toString());
      out.close();
    }
    catch(Exception ex)
    {
      System.out.println("Unable to write report file");
    }
  }

}
