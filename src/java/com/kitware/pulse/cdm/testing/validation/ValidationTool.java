/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.testing.validation;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.PrintWriter;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

import org.apache.poi.ss.usermodel.Cell;
import org.apache.poi.ss.usermodel.CellType;
import org.apache.poi.ss.usermodel.FormulaEvaluator;
import org.apache.poi.ss.usermodel.Row;
import org.apache.poi.xssf.usermodel.XSSFSheet;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;
import com.kitware.pulse.cdm.bind.Patient.PatientData.eSex;
import com.kitware.pulse.cdm.bind.Properties.ScalarData;
import com.kitware.pulse.cdm.bind.Validation.ValidationTargetData;
import com.kitware.pulse.cdm.bind.Validation.TimeSeriesValidationTargetData;
import com.kitware.pulse.cdm.bind.Validation.TimeSeriesValidationTargetListData;
import com.kitware.pulse.cdm.patient.SEPatient;
import com.kitware.pulse.cdm.patient.assessments.SEPatientAssessment;
import com.kitware.pulse.cdm.properties.SEScalar;
import com.kitware.pulse.utilities.DoubleUtils;
import com.kitware.pulse.utilities.FileUtils;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.RunConfiguration;
import com.kitware.pulse.utilities.StringUtils;
import com.kitware.pulse.utilities.UnitConverter;
import com.kitware.pulse.utilities.WaveformUtils;
import com.kitware.pulse.utilities.csv.CSVContents;

public abstract class ValidationTool
{
  public static void main(String[] args)
  {
    SystemValidation.main(args);
    //PatientValidation.main(args);
  }
  protected enum RunType { DOC, DATA };
  protected String DEST_DIRECTORY;
  protected String DEFAULT_DIRECTORY;
  protected String DEFAULT_FILE;
  protected String TABLE_TYPE;
  protected String VALIDATION_FOLDER;
  protected RunType RUN_TYPE=RunType.DOC;
  protected RunConfiguration cfg;

  public ValidationTool()
  {
    cfg = new RunConfiguration();
  }

  //Keywords success +/- 10% ,warning +/- 30%, danger+/-10%,fail
  protected static final String success = "<span class=\"success\">";
  protected static final String warning = "<span class=\"warning\">";
  protected static final String danger = "<span class=\"danger\">";
  protected static final String endSpan = "</span>";

  protected void CustomMarkdown(String sheetName, String destinationDirectory){};

  protected StringBuilder html = new StringBuilder();

  protected enum DataType { Min, Max, Mean, WaveformMin, WaveformMax, Enum, 
    Patient2SystemMean, Patient2SystemMin, Patient2SystemMax, 
    MinPerWeight, MaxPerWeight, MeanPerWeight, WaveformMinPerWeight, WaveformMaxPerWeight,
    MinPerIdealWeight, MaxPerIdealWeight, MeanPerIdealWeight, WaveformMinPerIdealWeight, WaveformMaxPerIdealWeight }
  // Patient2System variables take the value from the patient file and compare it to a system property
  // PerWeight calculates the expected value based on the patient weight

  protected boolean                         patientValidation=false;// Are we validating a patient or a system (with or without a patient)
  protected SEPatient                       patient;
  protected Map<String,List<Double>>        resultData = new HashMap<>();
  protected Map<String,SEPatientAssessment> assessments;

  protected class SummaryRow
  {
    public String str_success;
    public String str_warning;
    public String str_danger;
    public String str_total;
    public int num_success = 0;
    public int num_warning = 0;
    public int num_danger = 0;
    public int other=0;
    public int total=0;
  }
  protected class ValidationRow
  {
    public DataType dType = null;
    // xlsx values
    public String   name;   
    public String   unit = null; 
    public String   weightUnit = null;
    public String   refValues  = "";
    public String   refCites   = "";
    public double   refValue    = Double.NaN;
    public double   intervalMin = Double.NaN;
    public double   intervalMax = Double.NaN;
    public double   result = Double.NaN;
    public double   resultError;
    public String   resultEnum = "";
    public Boolean  patientSpecific = false;

    //  table headings
    public String   header   = "";
    public String   expected = "";
    public String   engine   = "";
    public String   error    = "";
    public String   notes    = "";
    public String   table    = "";

    // Results Data
    public String       assessmentFile = "";
    public List<Double> results;
    public List<Double> weight;
    public List<Double> idealWeight;

    public String doubleFormat="3f";
    public static final int successTolerance = 10;
    public static final int warningTolerance = 30;
  }

  public void populatePatientTab(XSSFWorkbook xlWBook, FormulaEvaluator evaluator) throws InvalidProtocolBufferException, NoSuchMethodException, SecurityException, IllegalAccessException, IllegalArgumentException, InvocationTargetException
  {
    XSSFSheet xlSheet = xlWBook.getSheetAt(0);
    evaluator.clearAllCachedResultValues();

    int rows = xlSheet.getPhysicalNumberOfRows();
    for (int r = 0; r < rows; r++) 
    {
      Row row = xlSheet.getRow(r);
      if (row == null) 
        continue;
      Cell cell = row.getCell(0);
      if(cell==null)
        continue;
      // Check to see if this row is a header
      String cellValue = cell.getStringCellValue();
      if(cellValue==null||cellValue.isEmpty())
        continue;// No property, skip it

      // Check to see if this has a patient table
      // We can push patient values to for dynamic validation
      if(cellValue.equals("Patient Inputs"))
      {
        for (r++; r < rows; r++)
        {
          row = xlSheet.getRow(r);
          Cell cName = row.getCell(0);
          Cell cUnits = row.getCell(1);
          Cell cValue = row.getCell(2);
          String name = cName.getStringCellValue();
          if(name == null||name.isEmpty())
            break;
          if(name.equals("Sex"))
          {
            if(patient == null)
            {
              Log.info("Using default gender of "+cValue.getStringCellValue());
              continue;// Just step through these to the end
            }
            String gender = patient.getSex()==eSex.Male?"Male":"Female";
            cValue.setCellValue(gender);
            Log.info("Setting patient gender to "+gender);
            continue;
          }
          if(cValue.getCellType() != CellType.NUMERIC)
          {
            evaluator.evaluateFormulaCell(cValue);
            Log.info("Using equation for "+name+ ", which computed value of "+cValue.getNumericCellValue()+cUnits.getStringCellValue());
            continue;// Skip equations
          }
          String units = "";
          if(cUnits != null)
            units = cUnits.getStringCellValue();
          if(patient == null)
          {
            Log.info("Using the default "+name+" of "+cValue.getNumericCellValue()+cUnits.getStringCellValue());
            continue;// Just step through these to the end
          }
          Method m = SEPatient.class.getMethod("get"+name);
          SEScalar s = ((SEScalar)m.invoke(patient));
          if(s.isValid())
          {
            double value = s.getValue(units);
            Log.info("Setting patient "+name+" to "+value+units);
            cValue.setCellValue(value);
          }
          else
            Log.error("Patient does not have a value for "+name+", using default in spreadsheet");
        }
        continue;
      }
    }
  }

  public void loadData(String csv_root)
  {
    FilenameFilter fileFilter = new FilenameFilter()
    {
      @Override
      public boolean accept(File f, String name)
      {
        if (name.endsWith(".csv"))
          return true;
        if(name.endsWith(".json") && name.contains("@"))
          return true;
        return false;
      }
    };

    String directoryName = DEFAULT_DIRECTORY;
    String fileName = DEFAULT_FILE;
    String destinationDirectory = DEST_DIRECTORY;
    String html_root = "./test_results/";
    if (csv_root.equals("TEST"))
      csv_root = "./test_results/scenarios/validation/"+VALIDATION_FOLDER+"/";
    else if(csv_root.equals("BASELINE"))
      csv_root = "./verification/scenarios/validation/"+VALIDATION_FOLDER+"/";
    else
    {
      html_root = csv_root + "/";
      Log.info("Looking for csv files in "+csv_root+"...");
      File f = new File(csv_root);
      if(f.list(fileFilter).length > 0)
      {
        if(!csv_root.endsWith("/"))
          csv_root = csv_root+"/";
        destinationDirectory = csv_root;
        RUN_TYPE = RunType.DATA;
      }
      else
      {
        Log.warn("No CSV files found, using baseline files for validation");
        csv_root = "./verification/scenarios/validation/"+VALIDATION_FOLDER+"/";
      }
    }

    try
    {
      if(!destinationDirectory.equals(csv_root))
      {
        File dest = new File(destinationDirectory);
        dest.mkdir();
        // Delete current dir contents
        // FileUtils.delete(destinationDirectory);
        // Ok, let's make them again
        // FileUtils.createDirectory(destinationDirectory);
      }
    }
    catch(Exception ex)
    {
      Log.error("Unable to clean directories");
      return;
    }

    try
    {   
      File xls = new File(directoryName+"/"+fileName);
      if(!xls.exists())
      {
        Log.error("Could not find xls file "+directoryName+"/"+fileName);
        return;
      }

      html.append("<html>");
      html.append("<body>");
      html.append("<h1>"+TABLE_TYPE+"Validation</h1><br>");

      // Get a list of all the results files we have to work with
      File vdir = new File(csv_root);
      String[] vFiles = vdir.list(fileFilter);
      if(vFiles==null || vFiles.length==0)
      {
        Log.error("No csv files were found, generate those please");
        return;
      }

      // Now read in the spreadsheet
      FileInputStream xlFile = new FileInputStream(directoryName+"/"+fileName);
      XSSFWorkbook xlWBook =  new XSSFWorkbook (xlFile);

      FormulaEvaluator evaluator = xlWBook.getCreationHelper().createFormulaEvaluator();
      List<ValidationRow> badSheets = new ArrayList<>();
      Map<String,List<ValidationRow>> tables = new HashMap<>();
      Map<String,List<ValidationRow>> tableErrors = new HashMap<>();
      List<ValidationRow> allRows = new ArrayList<>();
      Map<String,SummaryRow> summary = new TreeMap<>();

      //System sheets start at 1 (patient tab on sheet 0)
      //Patient sheets start at 0
      int startSheet = 1;
      if(TABLE_TYPE.equals("Patient"))
        startSheet = 0;
      for(int i=startSheet; i<xlWBook.getNumberOfSheets(); i++) 
      {
        XSSFSheet xlSheet = xlWBook.getSheetAt(i);
        Log.info("Processing Sheet : " + xlSheet.getSheetName());
        String sheetName = xlSheet.getSheetName().trim().replaceAll(" ", "");

        List<String> csvFiles = new ArrayList<>();
        String csvFile = sheetName+"ValidationResults.csv";
        File rFile = new File(csvFile);
        if(!rFile.exists())
        {
          // Search for any file starting with the sheet name
          for(String f : vFiles)
            if(f.startsWith(sheetName) && f.endsWith(".csv"))
              csvFiles.add(f);
        }
        else
          csvFiles.add(csvFile);

        for(String resultsName : csvFiles)
        {
          Log.info("Processing "+resultsName);
          // Is this patient validation?
          patient = null;
          if(TABLE_TYPE.equals("Patient") || RUN_TYPE == RunType.DATA || resultsName.contains("-"))
          {
            // Patient Name is encoded in the naming convention (or else it needs to be)
            String patientName = resultsName.substring(resultsName.lastIndexOf("-")+1,resultsName.indexOf("Results"));
            patient = new SEPatient();
            // Check to see if we have a patient file in our csv_root dir first
            File f = new File(csv_root+"patient.init.json");
            if(f.exists())
              patient.readFile(csv_root+"patient.init.json");
            else
              patient.readFile("./stable/"+patientName+".json");
          }
          else
          {
            return;
          }
          if(TABLE_TYPE.equals("System"))
            populatePatientTab(xlWBook, evaluator);
          try
          {
            // Look for a results file
            CSVContents results = new CSVContents(csv_root+resultsName);
            results.readAll(resultData);
            // Find any assessments
            assessments = new HashMap<>();
            for(String vFile : vFiles)
            {
              if(vFile.indexOf(sheetName)>-1 && vFile.indexOf('@')>-1)
              {
                try
                {
                  SEPatientAssessment ass = SEPatientAssessment.readAssessment(csv_root+vFile);
                  assessments.put(vFile,ass);
                }
                catch(InvalidProtocolBufferException ex)
                {
                  Log.error(vFile+" is named like a patient assessment, but its not?");
                }
              }
            }
          }
          catch(Exception ex)
          {
            ValidationRow vRow = new ValidationRow();
            vRow.header = sheetName;
            vRow.error=danger+"No results found for sheet "+endSpan;
            badSheets.add(vRow);
            continue;
          }

          allRows.clear();
          tables.clear();
          tableErrors.clear();
          // Read the sheet and process all the validation data rows
          try
          {
            int rows = xlSheet.getPhysicalNumberOfRows();
            for (int r = 0; r < rows; r++) 
            {
              Row row = xlSheet.getRow(r);
              if (row == null) 
                continue;
              int cells = 14;//row.getPhysicalNumberOfCells();
              Cell cell = row.getCell(0);
              if(cell==null)
                continue;
              // Check to see if this row is a header
              String cellValue = cell.getStringCellValue();
              if(cellValue==null||cellValue.isEmpty())
                continue;// No property, skip it

              if (row.getCell(1) != null)
                cellValue = row.getCell(1).getStringCellValue();
              if(cellValue!=null&&cellValue.equals("Units"))
                continue;// Header

              ValidationRow vRow = new ValidationRow();
              allRows.add(vRow);
              for (int c = 0; c <= cells; c++) 
              {
                cellValue = null;
                cell = row.getCell(c);
                if(cell==null)
                  continue;
                switch(cell.getCellType())
                {
                case NUMERIC:
                  cellValue = Double.toString(cell.getNumericCellValue());
                  break;
                case STRING:
                  cellValue = cell.getStringCellValue();
                  break;
                case FORMULA:
                  switch(evaluator.evaluateFormulaCell(cell))
                  {
                  case NUMERIC:
                    cellValue = String.format("%."+3+"g", cell.getNumericCellValue());
                    break;
                  case STRING:
                    cellValue = cell.getStringCellValue();
                    break;
                  default: // do nothing 
                  }
                default: // do nothing
                }

                switch(c)
                {
                case 0://A
                  Log.info("Processing "+cellValue);
                  vRow.name = cellValue.trim().replaceAll(" ","");
                  String prop = vRow.name;
                  if(vRow.name.indexOf('*')!=-1)
                    prop = prop.substring(0, prop.length()-1);
                  vRow.header = vRow.name;
                  break;
                case 1://B
                  if(cellValue!=null&&
                  !cellValue.equalsIgnoreCase("none")&&
                  !cellValue.equalsIgnoreCase("n\\a")&&
                  !cellValue.equalsIgnoreCase("n/a"))
                  {
                    vRow.unit = cellValue;
                  }
                  if(vRow.unit!=null&&!vRow.unit.isEmpty())
                    vRow.header+="("+vRow.unit+")";
                  break;
                case 2://C
                  if(cellValue!=null)
                  {
                    String unit=null;
                    int u = cellValue.indexOf("(");
                    if(u>-1)
                    {
                      unit = cellValue.substring(u+1, cellValue.indexOf(")"));
                      cellValue = cellValue.substring(0, u);
                    }
                    vRow.dType = DataType.valueOf(cellValue);
                    if(vRow.dType == DataType.MinPerWeight ||
                        vRow.dType == DataType.MaxPerWeight ||
                        vRow.dType == DataType.MeanPerWeight ||
                        vRow.dType == DataType.WaveformMinPerWeight || 
                        vRow.dType == DataType.WaveformMaxPerWeight ||  
                        vRow.dType == DataType.MinPerIdealWeight || 
                        vRow.dType == DataType.MaxPerIdealWeight ||
                        vRow.dType == DataType.MeanPerIdealWeight ||
                        vRow.dType == DataType.WaveformMinPerIdealWeight || 
                        vRow.dType == DataType.WaveformMaxPerIdealWeight)
                    {
                      vRow.weightUnit = unit;
                    }
                  }
                  break;
                case 3://D Male Multiplier
                case 4://E Female Multiplier
                  break;
                case 5://F Reference Values
                  // Replace any return characters with empty
                  if(patientValidation && vRow.name.indexOf('*')==-1)
                  {
                    try
                    {
                      Method has = SEPatient.class.getMethod("has"+vRow.name);
                      if((Boolean)has.invoke(patient))
                      {
                        Method get = SEPatient.class.getMethod("get"+vRow.name);
                        SEScalar s = ((SEScalar)get.invoke(patient));
                        vRow.refValue = s.getValue(vRow.unit);
                        vRow.refValues = cellValue;
                        break;
                      }
                      else
                      {
                        Log.error("Patient does not have a value for "+vRow.name);
                      }
                    }
                    catch(Exception ex)
                    {
                      // Nothing to do, row is not a patient property
                    }
                  }
                  if(cellValue==null)
                    vRow.refValues = null;
                  else
                    vRow.refValues = cellValue.replace("\n","");

                  break;
                case 6://G References
                  // Replace any return characters with empty
                  if(cellValue!=null)
                    cellValue = cellValue.replace("\n","");
                  vRow.refCites = cellValue;
                  break;
                case 7://H Reference Page (Internal only)
                  break;
                case 8://I PatientSpecific
                  if(cellValue!=null && (cellValue.toUpperCase().equals("Y")||cellValue.toUpperCase().equals("YES")))
                    vRow.patientSpecific = true;
                  break;
                case 9://J Notes
                  if(cellValue!=null)
                    vRow.notes = cellValue;
                  break;// Skipping for now
                case 10://K Internal Notes (Internal only)
                  break;
                case 11://L Reading (Internal only)
                  break;
                case 12://M Table (Internal only)
                  if(cellValue==null)
                    cellValue = "";
                  vRow.table = cellValue;
                  if(patientValidation)
                    vRow.table = patient.getName()+cellValue;
                  break;             
                case 13://N Table Mantissa Digits
                  if(cellValue!=null)
                    vRow.doubleFormat = cellValue;
                  if(patientValidation && vRow.dType != DataType.Patient2SystemMean)
                    vRow.refValues = String.format("%."+vRow.doubleFormat, vRow.refValue);
                  break;
                case 14://O Request Type (Internal only)
                  // We only handle if the request type is part of an assessment
                  if(cellValue!=null && cellValue.contains("@"))// @ denotes this property is part of an assessment
                    vRow.assessmentFile = cellValue;
                  break;
                }
              }
            }
          }
          catch(Exception ex)
          {
            Log.error("Error reading row");
            Log.error(ex.getMessage());
            ValidationRow vRow = new ValidationRow();
            vRow.header = sheetName;
            vRow.error=danger+"Sheet has errors"+endSpan;
            badSheets.add(vRow);
            continue;
          }

          // Sort all of our rows, and validate them
          for(ValidationRow vRow : allRows)
          {
            if(vRow.table.isEmpty())
              vRow.table = sheetName;//Default table is the sheet name
            if(!patientValidation && patient != null)
              vRow.table += "-" + patient.getName();
            if(!tables.containsKey(vRow.table))
              tables.put(vRow.table, new ArrayList<ValidationRow>());
            if(!tableErrors.containsKey(vRow.table))
              tableErrors.put(vRow.table, new ArrayList<ValidationRow>());
            if(buildExpectedHeader(vRow))
            {
              Log.info("Validating "+vRow.header); 
              if(validate(vRow))
              {
                tables.get(vRow.table).add(vRow);
              }
              else
                tableErrors.get(vRow.table).add(vRow);
            }
            else
              tableErrors.get(vRow.table).add(vRow);
          }

          for(String name : tables.keySet())
          {
            if(name.contains("All"))
              continue;
            List<ValidationRow> t = tables.get(name);
            AddToSummary(summary, name, t);
            WriteHTML(t,name);
            if(name.equalsIgnoreCase(sheetName))
            {
              List<String> properties = new ArrayList<>();
              for(ValidationRow vRow : t)  
                properties.add(vRow.name);
              for(ValidationRow vRow : tableErrors.get(name))  
                properties.add(vRow.name);
              CrossCheckValidationWithSchema(properties, tableErrors.get(name), name);
            }
            WriteHTML(tableErrors.get(name),name+"Errors");
            if(RUN_TYPE == RunType.DOC)
            {
              WriteDoxyTable(t,name,destinationDirectory);
              if(patientValidation)
                CustomMarkdown(patient.getName(),destinationDirectory);
            }
            if(RUN_TYPE == RunType.DATA)
            {
              if(!t.isEmpty())
              {
                if(patientValidation)
                  WriteValidationJson(t, csv_root+"Patient-"+patient.getName()+"-ValidationResults.json");
                else
                  WriteValidationJson(t, csv_root+name+"-ValidationResults.json");
              }
            }
          }
        }
      }
      xlWBook.close();
      if(RUN_TYPE == RunType.DOC)
        WriteSummary(summary, destinationDirectory);
      WriteHTML(badSheets,fileName+" Errors");
      html.append("</body>");
      html.append("</html>");
      try
      {
        BufferedWriter out = new BufferedWriter(new FileWriter(html_root+TABLE_TYPE+"Validation.html"));
        out.write(html.toString());
        out.close();
      }
      catch(Exception ex)
      {
        Log.error("Unable to write "+TABLE_TYPE+" HTML validation report");
        Log.error(ex.getMessage());
      }
    }
    catch(Exception ex)
    {
      Log.error("Error processing spreadsheet "+fileName);
      Log.error(ex.getMessage());
    }
    // Just for fun, I am going to create a single md file with ALL the tables in it
    if(RUN_TYPE == RunType.DOC)
    {
      try
      {
        String line;
        File vDir = new File(destinationDirectory);
        vDir.mkdir();
        String AllTableName = VALIDATION_FOLDER+"ValidationTables.md";
        PrintWriter writer = new PrintWriter(destinationDirectory+"/"+AllTableName, "UTF-8");

        for (String fName : vDir.list())
        {
          if(fName.equals(AllTableName))
            continue;
          if(new File(fName).isDirectory())
            continue;
          FileReader in = new FileReader(destinationDirectory+"/"+fName);
          BufferedReader inFile = new BufferedReader(in);
          writer.println(fName);
          while ((line = inFile.readLine()) != null)
            writer.println(line);
          inFile.close();
          inFile = null;
          writer.println("<br>");
        }
        writer.close();
        writer = null;
        System.gc();
      }
      catch (Exception ex)
      {
        Log.error("Unable to create single validation table file.");
        Log.error(ex.getMessage());
      }
    }
  }

  protected boolean buildExpectedHeader(ValidationRow vRow)
  {
    //if(vRow.name.startsWith("Bladder-Lactate-Concentration"))
    // Log.info("Here");
    if(vRow.name.contains("*"))
    {
      vRow.error = warning+"Not Validating"+endSpan;
      return false;
    }
    if(vRow.refValues==null||vRow.refCites==null)
    {
      vRow.error = danger+"No reference provided"+endSpan;
      return true;
    }
    String[] refs  = vRow.refCites.trim().split("\\s*,\\s*");
    String[] rVals = vRow.refValues.trim().split("\\s*,\\s*");
    // Check to see if we have 1 value
    if(rVals.length==1)
    {
      if(refs.length!=1)
      {
        vRow.expected = "1 reference value should have 1 reference";
        return false;
      }
      String cite = "@cite ";
      if(vRow.refCites.contains("["))
        cite = "";// This is an equation, not a cite
      if(vRow.dType==DataType.Min || vRow.dType==DataType.Max || vRow.dType==DataType.Mean ||
          vRow.dType == DataType.MinPerWeight || vRow.dType == DataType.MaxPerWeight || vRow.dType == DataType.MeanPerWeight ||
          vRow.dType == DataType.MinPerIdealWeight ||vRow.dType == DataType.MaxPerIdealWeight ||vRow.dType == DataType.MeanPerIdealWeight ||
          vRow.dType==DataType.WaveformMin ||  vRow.dType == DataType.WaveformMinPerWeight || vRow.dType == DataType.WaveformMinPerIdealWeight ||
          vRow.dType==DataType.WaveformMax ||  vRow.dType == DataType.WaveformMaxPerWeight || vRow.dType == DataType.WaveformMaxPerIdealWeight)
      {
        try{vRow.refValue = Double.parseDouble(vRow.refValues);}
        catch(Exception ex)
        {
          Log.error(vRow.name +" could not parse reference value "+vRow.refValues);
        }
        if(vRow.refCites.equals("N/A"))
          vRow.expected = String.format("%."+vRow.doubleFormat, vRow.refValue);
        else
          vRow.expected = String.format("%."+vRow.doubleFormat, vRow.refValue)+" "+cite+vRow.refCites;

        if(vRow.dType==DataType.MeanPerWeight || vRow.dType==DataType.MeanPerIdealWeight ||
            vRow.dType==DataType.WaveformMinPerWeight || vRow.dType==DataType.WaveformMaxPerWeight ||
            vRow.dType==DataType.WaveformMinPerIdealWeight || vRow.dType==DataType.WaveformMaxPerIdealWeight)
        {
          if(vRow.header.endsWith(")"))
            vRow.header = vRow.header.substring(0, vRow.header.length()-1) + "/" +vRow.weightUnit + ")";
          else
            vRow.header = vRow.header + "/" +vRow.weightUnit;
        }               
      }
      else if(vRow.dType==DataType.Patient2SystemMean || vRow.dType==DataType.Patient2SystemMax || vRow.dType==DataType.Patient2SystemMin)
      {        
        vRow.header = vRow.name;   
        vRow.name = vRow.refValues;
        if(vRow.dType == DataType.Patient2SystemMax)
          vRow.header += " Maximum";
        if(vRow.dType == DataType.Patient2SystemMin)
          vRow.header += " Minimum";
        if(vRow.unit!=null && !vRow.unit.isEmpty())
          vRow.header += "("+vRow.unit+")";
        vRow.expected = String.format("%."+vRow.doubleFormat, vRow.refValue);
      }
      else if(!vRow.refValues.isEmpty())
      {
        if(vRow.refCites.equals("N/A"))
          vRow.expected = vRow.refValues;
        else
          vRow.expected = vRow.refValues+" "+cite+vRow.refCites;
      }

      return true;
    }
    // Check to see if vRow.refValues is an interval, which is always form of [x,y]

    char start = vRow.refValues.charAt(0);
    if( rVals.length==2 && start=='[')
    {// We are an interval range   
      if(refs.length!=1)
      {
        vRow.expected = "Need a single reference for an interval";
        return false;
      }
      int sep = vRow.refValues.indexOf(",");
      if(sep==-1)
      {
        vRow.expected = "Malformed interval string : "+vRow.refValues;
        return false;
      }
      if(vRow.refValues.indexOf("]")<sep)
      {
        vRow.expected = "Malformed interval string : "+vRow.refValues;
        return false;
      }
      vRow.intervalMin = Double.parseDouble(vRow.refValues.substring(1, sep));
      vRow.intervalMax = Double.parseDouble(vRow.refValues.substring(sep+1,vRow.refValues.length()-1));   
      String cite = "@cite ";
      if(vRow.refCites.contains("["))
        cite = "";// This is an equation, not a cite
      vRow.expected = vRow.refValues+" "+cite+vRow.refCites;
      return true;
    }
    else
    {// It's more than one value,intervals, this is where things get tricky.
      List<String> cites = new ArrayList<>();
      List<Double> values = new ArrayList<>();

      int cIdx=0;
      for(int i=0; i<rVals.length; i++)
      {
        if(rVals[i].startsWith("["))
        {
          cites.add(refs[cIdx]);
          try
          {values.add(Double.parseDouble(rVals[i].substring(1)));}
          catch(Exception ex)
          {
            Log.error(vRow.header + " has string formatting error : " + vRow.refValues);
            return false;
          }
          i++;
          if(!rVals[i].endsWith("]"))
          {
            vRow.expected = "Malformed interval string : "+vRow.refValues;
            return false;
          }
          cites.add(refs[cIdx]);
          values.add(Double.parseDouble(rVals[i].substring(0, rVals[i].length()-1)));
          cIdx++;
          continue;
        }
        cites.add(refs[cIdx]);
        values.add(Double.parseDouble(rVals[i]));
        cIdx++;
      }

      int minIdx=0, maxIdx=0;
      vRow.intervalMin = Double.MAX_VALUE;
      vRow.intervalMax = -Double.MAX_VALUE;
      for(int i=0; i<values.size(); i++)
      {
        double d = values.get(i);
        if(d < vRow.intervalMin)
        {
          minIdx = i;
          vRow.intervalMin = d;
        }
        if(d > vRow.intervalMax)
        {
          maxIdx = i;
          vRow.intervalMax = d;
        }
      }
      String cite = "@cite ";
      if(cites.get(minIdx).contains("["))
        cite = "";// This is an equation, not a cite
      if(cites.get(minIdx).equals(cites.get(maxIdx)))
        vRow.expected = "["+vRow.intervalMin+", "+vRow.intervalMax+"] "+cite+cites.get(minIdx);
      else// We should never have two different calculations for our interval... but if we do, you got more coding to do!
        vRow.expected = "["+vRow.intervalMin+" @cite "+cites.get(minIdx)+", "+vRow.intervalMax+" @cite "+cites.get(maxIdx)+"]";

      return true;
    }
  }

  protected boolean validate(ValidationRow vRow)
  {
    if(vRow.name.startsWith("Bladder-Lactate-Concentration"))
      Log.info("Break");
    if(!getResult(vRow))
    {
      return false;
    }
    // Waveform are not great to use for respiratory system
    // We changed to a simple min/max from the local average of all min/max
    // This is due to sub min/max skewing results (ex. Expiratory Flow)
    //    ____
    //   /    \ __
    //  /      V  \  <-- That second smaller waveform lowers the overall average
    // /           \
    // Waveforms also assume a constant amplitude (All validation is on resting waveforms)
    // So they are probably ok to use in CV, but not respiratory validation
    switch(vRow.dType)
    {
    case Mean:
    case Patient2SystemMean:
    {
      vRow.result = DoubleUtils.getAverage(vRow.results);
      vRow.engine = "Mean of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }      
    case Min:
    {
      vRow.result = DoubleUtils.getMin(vRow.results);
      vRow.engine = "Minimum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case WaveformMin:
    {
      vRow.result = WaveformUtils.getMin(vRow.results);
      vRow.engine = "Minimum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case Patient2SystemMin:
    {
      vRow.result = DoubleUtils.getMin(vRow.results);
      vRow.engine = "Minimum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }      
    case Max:
    {
      vRow.result = DoubleUtils.getMax(vRow.results);
      vRow.engine = "Maximum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case WaveformMax:
    {
      vRow.result = WaveformUtils.getMax(vRow.results);
      vRow.engine = "Maximum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case Patient2SystemMax:
    {
      vRow.result = DoubleUtils.getMax(vRow.results);
      vRow.engine = "Maximum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }      
    case Enum:
    {
      vRow.engine = vRow.resultEnum;
      break;
    }
    case MinPerWeight:
    {
      vRow.result = DoubleUtils.getMin(vRow.results) / DoubleUtils.getAverage(vRow.weight);
      vRow.engine = "Minimum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case MaxPerWeight:
    {
      vRow.result = DoubleUtils.getMax(vRow.results) / DoubleUtils.getAverage(vRow.weight);
      vRow.engine = "Maximum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case MeanPerWeight:
    {
      List<Double> resultPerWeight = new ArrayList<>();
      for(int i=0; i<vRow.results.size(); i++)
      {
        resultPerWeight.add(vRow.results.get(i) / vRow.weight.get(i));
      }
      vRow.result = DoubleUtils.getAverage(resultPerWeight);
      vRow.engine = "Mean of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case WaveformMinPerWeight:
    {
      List<Double> xMin = new ArrayList<>();
      List<Double> yMin = new ArrayList<>();
      List<Double> xMax = new ArrayList<>();
      List<Double> yMax = new ArrayList<>();
      WaveformUtils.getPeriodBounds(vRow.weight, vRow.results, xMin, yMin, xMax, yMax);
      List<Double> resultPerWeight = new ArrayList<>();
      for(int i=0; i<yMin.size(); i++)
      {
        resultPerWeight.add(yMin.get(i) / xMin.get(i));
      }
      vRow.result = DoubleUtils.getMin(resultPerWeight);
      vRow.engine = "Minimum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case WaveformMaxPerWeight:
    {
      List<Double> xMin = new ArrayList<>();
      List<Double> yMin = new ArrayList<>();
      List<Double> xMax = new ArrayList<>();
      List<Double> yMax = new ArrayList<>();    
      WaveformUtils.getPeriodBounds(vRow.weight, vRow.results, xMin, yMin, xMax, yMax);
      List<Double> resultPerWeight = new ArrayList<>();
      for(int i=0; i<yMax.size(); i++)
      {
        resultPerWeight.add(yMax.get(i) / xMax.get(i));
      }
      vRow.result = DoubleUtils.getMax(resultPerWeight);
      vRow.engine = "Maximum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case MinPerIdealWeight:
    {
      vRow.result = DoubleUtils.getMin(vRow.results) / DoubleUtils.getAverage(vRow.idealWeight);
      vRow.engine = "Minimum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case MeanPerIdealWeight:
    {
      List<Double> resultPerWeight = new ArrayList<>();
      for(int i=0; i<vRow.results.size(); i++)
      {
        resultPerWeight.add(vRow.results.get(i) / vRow.idealWeight.get(i));
      }
      vRow.result = DoubleUtils.getAverage(resultPerWeight);
      vRow.engine = "Mean of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case MaxPerIdealWeight:
    {
      vRow.result = DoubleUtils.getMax(vRow.results) / DoubleUtils.getAverage(vRow.idealWeight);
      vRow.engine = "Maximum of " + String.format("%."+vRow.doubleFormat, vRow.result); //StringUtils.toString(vRow.mean,3);
      break;
    }
    case WaveformMinPerIdealWeight:
    {
      List<Double> xMin = new ArrayList<>();
      List<Double> yMin = new ArrayList<>();
      List<Double> xMax = new ArrayList<>();
      List<Double> yMax = new ArrayList<>();
      WaveformUtils.getPeriodBounds(vRow.idealWeight, vRow.results, xMin, yMin, xMax, yMax);
      List<Double> resultPerWeight = new ArrayList<>();
      for(int i=0; i<yMin.size(); i++)
      {
        resultPerWeight.add(yMin.get(i) / xMin.get(i));
      }
      vRow.result = DoubleUtils.getMin(resultPerWeight);
      break;
    }
    case WaveformMaxPerIdealWeight:
    {
      List<Double> xMin = new ArrayList<>();
      List<Double> yMin = new ArrayList<>();
      List<Double> xMax = new ArrayList<>();
      List<Double> yMax = new ArrayList<>();
      WaveformUtils.getPeriodBounds(vRow.idealWeight, vRow.results, xMin, yMin, xMax, yMax);
      List<Double> resultPerWeight = new ArrayList<>();
      for(int i=0; i<yMax.size(); i++)
      {
        resultPerWeight.add(yMax.get(i) / xMax.get(i));
      }
      vRow.result = DoubleUtils.getMax(resultPerWeight);
      break;
    }
    default:
    {
      vRow.error =  danger+"Unsupported data type "+vRow.dType+endSpan;
      return false;
    }
    }
    if(Double.isNaN(vRow.result) && vRow.resultEnum.isEmpty())
    {
      vRow.error = danger+"Not found in result"+endSpan;
      return false;
    }
    // Round our result value to it's specified precision
    vRow.result = Double.parseDouble(String.format("%."+vRow.doubleFormat, vRow.result));

    if(!Double.isNaN(vRow.intervalMin)&&!Double.isNaN(vRow.intervalMax))
    {     
      if(vRow.result < vRow.intervalMin)
      {
        vRow.resultError = -DoubleUtils.percentTolerance(vRow.intervalMin, vRow.result, 1e-10);
      }
      else if(vRow.result > vRow.intervalMax)
      {
        vRow.resultError = DoubleUtils.percentTolerance(vRow.intervalMax, vRow.result, 1e-10);
      }
      else
      {
        vRow.error = success+"Within bounds"+endSpan;
        return true;
      }
    }
    else if(!Double.isNaN(vRow.refValue))
    {// What is the % difference between the value and the test value
      if(vRow.dType == DataType.Patient2SystemMax && vRow.result <= vRow.refValue)
      {
        vRow.resultError = 0;
        vRow.error  = success+"Under Max"+endSpan;
        return true;
      }
      if(vRow.dType == DataType.Patient2SystemMin && vRow.result >= vRow.refValue)
      {
        vRow.resultError = 0;
        vRow.error  = success+"Above Min"+endSpan;
        return true;
      }      
      vRow.resultError = DoubleUtils.percentTolerance(vRow.refValue, vRow.result, 1e-10);
      if(vRow.result < vRow.refValue)
        vRow.resultError = -vRow.resultError;
    }
    else if(vRow.dType == DataType.Enum)
    {
      if(vRow.resultEnum.equalsIgnoreCase(vRow.engine))
      {
        vRow.error  = success+"Match"+endSpan;
      }
      else
      {
        vRow.error  = danger+"No Match"+endSpan;
      }
      return true;
    }
    else
    {
      if(vRow.error.isEmpty())
        vRow.error =  danger+"Unsupported reference value"+endSpan;
      return false;
    }

    if(Double.isNaN(vRow.resultError) || Double.isInfinite(vRow.resultError))
    {
      vRow.error  = "N/A : divide by 0";
    }
    else if(Double.isInfinite(vRow.resultError))
    {
      vRow.error  = "N/A : divide by 0";
    }
    else
    {    
      String span;
      double abs = Math.abs(vRow.resultError);
      if(abs<ValidationRow.successTolerance)
        span = success;
      else if(abs<ValidationRow.warningTolerance)
        span = warning; 
      else
        span = danger;
      vRow.error  = span+StringUtils.toString(vRow.resultError,1)+"%"+endSpan;
    }

    return true;
  }

  protected boolean getResult(ValidationRow vRow)
  {
    //if(vRow.name.startsWith("Bladder-Lactate-Concentration"))
    //  Log.info("Here");

    if(vRow.dType == DataType.MinPerIdealWeight || vRow.dType == DataType.MaxPerIdealWeight || vRow.dType == DataType.MeanPerIdealWeight|| 
        vRow.dType == DataType.WaveformMinPerIdealWeight || vRow.dType == DataType.WaveformMaxPerIdealWeight)
    {// Take off the weight unit from the header and unit we got from the spreadsheet
      String weightHeader = "Patient-IdealBodyWeight("+vRow.weightUnit+")";
      vRow.idealWeight = resultData.get(weightHeader);
      if(vRow.idealWeight==null)
      {// Maybe the result is in a different unit than our reference value
        String newUnit=null;
        String[] split;
        for(String result : resultData.keySet())
        {
          split = result.split("[(]");
          if(split.length==2&&split[0].equals("Patient-IdealBodyWeight"))
          {
            newUnit = split[1].substring(0, split[1].indexOf(')'));
            vRow.idealWeight = resultData.get(result);
            for(int i=0; i<vRow.idealWeight.size(); i++)
            {
              double newValue = UnitConverter.convert(vRow.idealWeight.get(i), newUnit, vRow.weightUnit);
              vRow.idealWeight.set(i, newValue);
            }
            // Since we did a conversion, let's update the key
            resultData.remove(result);
            result = split[0]+"("+vRow.weightUnit+")";
            resultData.put(result,vRow.idealWeight);
            break;
          }
        }
        if(newUnit==null)// Nope, I don't think the result is there
        {
          vRow.error = danger+"Patient-IdealWeight, Not In Results!"+endSpan;
          return false;
        }
      } 
    }

    if(vRow.dType == DataType.MinPerWeight || vRow.dType == DataType.MaxPerWeight || vRow.dType == DataType.MeanPerWeight || 
        vRow.dType == DataType.WaveformMinPerWeight || vRow.dType == DataType.WaveformMaxPerWeight)
    {// Take off the weight unit from the header and unit we got from the spreadsheet
      String weightHeader = "Patient-Weight("+vRow.weightUnit+")";
      vRow.weight = resultData.get(weightHeader);
      if(vRow.weight==null)
      {// Maybe the result is in a different unit than our reference value
        String newUnit=null;
        String[] split;
        for(String result : resultData.keySet())
        {
          split = result.split("[(]");
          if(split.length==2&&split[0].equals("Patient-Weight"))
          {
            newUnit = split[1].substring(0, split[1].indexOf(')'));
            vRow.weight = resultData.get(result);
            for(int i=0; i<vRow.weight.size(); i++)
            {
              double newValue = UnitConverter.convert(vRow.weight.get(i), newUnit, vRow.weightUnit);
              vRow.weight.set(i, newValue);
            }
            // Since we did a conversion, let's update the key
            resultData.remove(result);
            result = split[0]+"("+vRow.weightUnit+")";
            resultData.put(result,vRow.weight);
            break;
          }
        }
        if(newUnit==null)// Nope, I don't think the result is there
        {
          vRow.error = danger+"Patient-Weight, Not In Results!"+endSpan;
          return false;
        }
      } 
    }

    if(vRow.assessmentFile.indexOf('@')>-1)
    {
      // It's an assessment row
      for(String s : assessments.keySet())
      {
        if(s.indexOf(vRow.assessmentFile)>-1)
        {
          try
          {
            SEPatientAssessment a = assessments.get(s);
            Method m = a.getClass().getMethod("get"+vRow.name);
            Object r = m.invoke(a);
            if(r instanceof SEScalar)
            {
              SEScalar scalar = (SEScalar)m.invoke(a);
              if(!scalar.isValid())
              {
                vRow.error = danger+vRow.name+", Not Valid In Assessment!"+endSpan;
                return false;
              }
              vRow.results = new ArrayList<>();
              vRow.results.add(scalar.getValue(vRow.unit));
            }
            else if(r.getClass().isEnum())
            {// TODO will need to get the enum value at some point
              vRow.resultEnum = r.toString();
            }
            return true;
          }
          catch(Exception ex)
          {
            vRow.error = danger+vRow.name+", Not In Assessment!"+endSpan;
            return false;
          }
        }
      }
      vRow.error = danger+vRow.name+", was not found in Assessment!"+endSpan;
      return false;
    }

    vRow.results = getResults(vRow.name,vRow.unit);
    if(vRow.results==null && patientValidation)
      vRow.results = getResults("Patient-"+vRow.name,vRow.unit);// See if this a patient output
    if(vRow.results==null)// Nope, I don't think the result is there
    {
      vRow.error = danger+"Not In Results!"+endSpan;
      return false;
    }
    return true;
  }

  protected List<Double> getResults(String name, String unit)
  {
    //if(name.equals("BloodDensity"))
    //  Log.info("Here");
    String header = name;
    if(unit!=null && !unit.isEmpty())
      header = name+"("+unit+")";
    List<Double> results = resultData.get(header);
    if(results==null)
    {// Maybe the result is in a different unit than our reference value
      String outputUnit=null;
      String[] split;
      for(String result : resultData.keySet())
      {
        split = result.split("[(]");
        if(split.length==2&&split[0].equals(name))
        {
          outputUnit = split[1].substring(0, split[1].indexOf(')'));
          results = resultData.get(result);
          for(int i=0; i<results.size(); i++)
          {
            if(!UnitConverter.isCompatibleWithUnit(outputUnit, unit))
            {
              Log.error(name + " has incompatible units between what was generated " + outputUnit + " and the validation unit " + unit);
              return null;
            }
            double newValue = UnitConverter.convert(results.get(i), outputUnit, unit);
            results.set(i, newValue);
          }
          // Since we did a conversion, let's update the key
          resultData.remove(result);
          result = split[0]+"("+unit+")";
          resultData.put(result,results);
          break;
        }
      }
    } 
    return results;
  }

  protected void AddToSummary(Map<String,SummaryRow> summary, String sheetName, List<ValidationRow> vData)
  {
    SummaryRow row = new SummaryRow();
    for(ValidationRow vRow : vData)
    {
      row.total++;
      double abs = Math.abs(vRow.resultError);
      if(Double.isNaN(vRow.resultError))
        row.other++;
      else if(abs < ValidationRow.successTolerance)
        row.num_success++;
      else if(abs < ValidationRow.warningTolerance)
        row.num_warning++;
      else
        row.num_danger++;
    }
    row.str_success = ValidationTool.success + Integer.toString(row.num_success) + ValidationTool.endSpan;
    row.str_warning = ValidationTool.warning + Integer.toString(row.num_warning) + ValidationTool.endSpan;
    row.str_danger = ValidationTool.danger + Integer.toString(row.num_danger) + ValidationTool.endSpan;
    if(sheetName.contains("Patient"))
      sheetName = sheetName.substring(0,sheetName.length()-7);
    row.str_total = Integer.toString(row.total);
    summary.put("%"+sheetName,row);
  }
  protected void WriteSummary(Map<String,SummaryRow> summary, String destinationDirectory)
  {
    if(summary.isEmpty())
      return;
    String columnHeaders[] = new String[5];
    int maxColumnLength[] = new int[columnHeaders.length];
    columnHeaders[0] = "Category";
    columnHeaders[1] = " < "+Integer.toString(ValidationRow.successTolerance)+"% ";
    columnHeaders[2] = " "+Integer.toString(ValidationRow.successTolerance)+"%-"+Integer.toString(ValidationRow.warningTolerance)+"%";
    columnHeaders[3] = " > "+Integer.toString(ValidationRow.warningTolerance)+"% ";
    columnHeaders[4] = " Total ";
    for(int i=0; i<maxColumnLength.length; i++)
      maxColumnLength[i] = columnHeaders[i].length();
    // Size the category column appropriately 
    for(String sName : summary.keySet())
    {
      SummaryRow sRow = summary.get(sName);
      if(sName.length()>maxColumnLength[0])
        maxColumnLength[0] = sName.length();
      if(sRow.str_success.length()>maxColumnLength[1])
        maxColumnLength[1] = sRow.str_success.length();
      if(sRow.str_warning.length()>maxColumnLength[2])
        maxColumnLength[2] = sRow.str_warning.length();
      if(sRow.str_danger.length()>maxColumnLength[3])
        maxColumnLength[3] = sRow.str_danger.length();
      if(sRow.str_total.length()>maxColumnLength[4])
        maxColumnLength[4] = sRow.str_total.length();
    }

    String tableName = TABLE_TYPE + "Summary";

    int success = 0;
    int warning = 0;
    int danger  = 0;
    int total   = 0;
    PrintWriter writer=null;
    try
    {
      // Create file and start the table
      writer = new PrintWriter(destinationDirectory+"/"+tableName+"ValidationTable.md", "UTF-8");
      for(int i=0; i<columnHeaders.length; i++)
        writer.print("|"+pad(columnHeaders[i],maxColumnLength[i]));
      writer.println("|");
      for(int i=0; i<columnHeaders.length; i++)
        writer.print("|"+pad("---",maxColumnLength[i]));
      writer.println("|");
      // Now loop the vData and write out table rows
      for(String sName : summary.keySet())
      {
        SummaryRow sRow = summary.get(sName);
        writer.print("|"+pad(sName,maxColumnLength[0]));
        writer.print("|"+pad(sRow.str_success,maxColumnLength[1]));
        writer.print("|"+pad(sRow.str_warning,maxColumnLength[2]));
        writer.print("|"+pad(sRow.str_danger,maxColumnLength[3]));
        writer.print("|"+pad(sRow.str_total,maxColumnLength[4]));
        writer.println("|");
        success += sRow.num_success;
        warning += sRow.num_warning;
        danger += sRow.num_danger;
        total += sRow.total;
      }
      String str_success = ValidationTool.success + Integer.toString(success) + ValidationTool.endSpan;
      String str_warning = ValidationTool.warning + Integer.toString(warning) + ValidationTool.endSpan;
      String str_danger = ValidationTool.danger + Integer.toString(danger) + ValidationTool.endSpan;
      String str_total = Integer.toString(total);
      writer.print("|"+pad("Total",maxColumnLength[0]));
      writer.print("|"+pad(str_success,maxColumnLength[1]));
      writer.print("|"+pad(str_warning,maxColumnLength[2]));
      writer.print("|"+pad(str_danger,maxColumnLength[3]));
      writer.print("|"+pad(str_total,maxColumnLength[4]));
      writer.println("|");

      writer.close();
    }
    catch(Exception ex)
    {
      Log.error("Error writing validation table for "+tableName);
      Log.error(ex.getMessage());
      writer.close();
    }
  }

  protected void WriteDoxyTable(List<ValidationRow> vData, String sheetName, String destinationDirectory)
  {
    if(vData.isEmpty())
      return;
    String columnHeaders[] = new String[5];
    int maxColumnLength[] = new int[columnHeaders.length];
    columnHeaders[0] = "Property Name";
    columnHeaders[1] = "Expected Value";
    columnHeaders[2] = "Engine Value";
    columnHeaders[3] = "Percent Error";
    columnHeaders[4] = "Notes";
    for(int i=0; i<maxColumnLength.length; i++)
      maxColumnLength[i] = columnHeaders[i].length();

    // Compute all of our header values and sizes
    for(ValidationRow vRow : vData)
    {
      // Header (was built when we had to pull data out of the results file)
      if(vRow.header.length()>maxColumnLength[0])
        maxColumnLength[0] = vRow.header.length();
      if(vRow.expected.length()>maxColumnLength[1])
        maxColumnLength[1] = vRow.expected.length();
      if(vRow.engine.length()>maxColumnLength[2])
        maxColumnLength[2] = vRow.engine.length();
      if(vRow.error.length()>maxColumnLength[3])
        maxColumnLength[3] = vRow.error.length();
      if(vRow.notes.length()>maxColumnLength[4])
        maxColumnLength[4] = vRow.notes.length();
    }

    PrintWriter writer=null;
    try
    {
      // Create file and start the table
      writer = new PrintWriter(destinationDirectory+"/"+sheetName+"ValidationTable.md", "UTF-8");
      for(int i=0; i<columnHeaders.length; i++)
        writer.print("|"+pad(columnHeaders[i],maxColumnLength[i]));
      writer.println("|");
      for(int i=0; i<columnHeaders.length; i++)
        writer.print("|"+pad("---",maxColumnLength[i]));
      writer.println("|");
      // Now loop the vData and write out table rows
      for(ValidationRow vRow : vData)
      {
        writer.print("|"+pad(vRow.header,maxColumnLength[0]));
        writer.print("|"+pad(vRow.expected,maxColumnLength[1]));
        writer.print("|"+pad(vRow.engine,maxColumnLength[2]));
        writer.print("|"+pad(vRow.error,maxColumnLength[3]));
        writer.print("|"+pad(vRow.notes,maxColumnLength[4]));
        writer.println("|");
      }

      writer.close();
    }
    catch(Exception ex)
    {
      Log.error("Error writing validation table for "+sheetName);
      Log.error(ex.getMessage());
      writer.close();
    }
  }

  protected String pad(String s, int max)
  {
    if(s==null)
      return new String(new char[(max+5)]).replace('\0',' ');
    // Using a pad of 5 between columns
    try
    {
      String ret =  s + new String(new char[(max-s.length())+5]).replace('\0',' ');
      return ret;
    }
    catch(Exception ex)
    {
      Log.error("Could not pad "+s+" with a max of "+max);
      Log.error(ex.getMessage());
      return "";
    }
  }

  // TODO this should also cross check that we have everything accounted for with assessments too
  protected void CrossCheckValidationWithSchema(List<String> properties, List<ValidationRow> eData, String sheetName)
  {
    // Remove any * in the properties list
    for(int i=0; i<properties.size(); i++)
    {
      String p = properties.get(i);
      int a = p.indexOf('*');
      if(a>-1)
        properties.set(i, p.substring(0, a));
    }

    Class<?> c = null;
    try
    {
      if(TABLE_TYPE.equals("System"))
        c = Class.forName("com.kitware.pulse.cdm.bind.Physiology$"+sheetName+"SystemData");
      else if(TABLE_TYPE.equalsIgnoreCase("Patient"))
        c = Class.forName("com.kitware.pulse.cdm.bind.Patient.PatientData");
      else
        throw new RuntimeException("Unknown table type "+TABLE_TYPE);
    }
    catch(Exception ex)
    {
      Log.error("Cannot map sheet "+sheetName+" to a CDM class.");
      Log.error(ex.getMessage());
      return;
    }
    Set<String> cdm = new HashSet<>();
    // Grab all scalar data property names from the bind class
    for(Method m : c.getMethods())
    {
      if(m.getName().startsWith("get")&&ScalarData.class.isAssignableFrom(m.getReturnType()))
      {
        boolean found=false;
        String prop = m.getName().substring(3);
        cdm.add(prop);
        for(String p : properties)
        {
          if(p.equals(prop))
          {
            found=true;
            break;
          }
        }
        if(!found)
        {// Missing a row for this property
          ValidationRow row = new ValidationRow();
          row.name = row.header = prop;
          row.error = danger+"Missing entry for property"+endSpan;
          eData.add(row);
        }
      }
    }

    for(String property : properties)
    {      
      if(!property.contains("-")&&!cdm.contains(property))
      {
        try
        {// Maybe this is a non Scalar method...
          c.getMethod("get"+property);
        }
        catch(Exception ex)
        {
          try
          {// Or a boolean method...
            c.getMethod("is"+property);
          }
          catch(Exception e2)
          {
            ValidationRow row = new ValidationRow();
            row.name = row.header = property;
            row.error = danger+"Is not in the CDM"+endSpan;
            eData.add(row);
          }
        }

      }
    }
  }

  protected void WriteHTML(List<ValidationRow> vData, String tableName)
  {
    html.append("<br><br>");

    html.append("<table border=\"1\">");
    html.append("<tr>");
    html.append("<th> "+tableName+" </th>");
    html.append("<th> Expected Value </th>");
    html.append("<th> Engine Value </th>");
    html.append("<th> Percent Error </th>");
    html.append("<th> Notes </th>");
    html.append("</tr>");

    for(ValidationRow vRow : vData)
    {
      if(vRow.error.contains(success))
        html.append("<tr bgcolor=\"#00FF00\">");
      else if(vRow.error.contains(warning))
        html.append("<tr bgcolor=\"#FFFF00\">");
      else if(vRow.error.contains(danger))
        html.append("<tr bgcolor=\"#FF0000\">");
      else
        html.append("<tr bgcolor=\"#FFFFFF\">");
      html.append("<td>"+vRow.header+"</td>");
      html.append("<td>"+vRow.expected+"</td>");
      html.append("<td>"+vRow.engine+"</td>");
      html.append("<td>"+vRow.error+"</td>");
      html.append("<td>"+vRow.notes+"</td>");
    }
    html.append("</table>");
  }
  protected void WriteValidationJson(List<ValidationRow> vData, String filepath) throws InvalidProtocolBufferException
  {
    TimeSeriesValidationTargetListData.Builder vtList = TimeSeriesValidationTargetListData.newBuilder();
    for(ValidationRow vRow : vData)
    {
      TimeSeriesValidationTargetData.Builder pvd = vtList.addTimeSeriesValidationTargetBuilder();
      ValidationTargetData.Builder vtd = pvd.getValidationTargetBuilder();
      vtd.setHeader(vRow.header+"-"+vRow.dType.toString());
      pvd.setEqualToValue(vRow.refValue);
      pvd.setComputedValue(vRow.result);
      pvd.setError(vRow.resultError);
      pvd.setPatientSpecific(vRow.patientSpecific);
      
    }
    vtList.build();
    FileUtils.writeFile(filepath, JsonFormat.printer().print(vtList));
  }
}
