/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.doxygen;

import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

import com.kitware.pulse.cdm.bind.AnesthesiaMachine.AnesthesiaMachineData;
import com.kitware.pulse.cdm.bind.Enums.eBreathState;
import com.kitware.pulse.cdm.bind.Enums.eCharge;
import com.kitware.pulse.cdm.bind.Enums.eDriverWaveform;
import com.kitware.pulse.cdm.bind.Enums.eGate;
import com.kitware.pulse.cdm.bind.Enums.eSide;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.Environment.EnvironmentalConditionsData;
import com.kitware.pulse.cdm.bind.Events.eEvent;
import com.kitware.pulse.cdm.bind.Patient.PatientData;
import com.kitware.pulse.cdm.bind.PatientActions.BrainInjuryData;
import com.kitware.pulse.cdm.bind.PatientActions.HemorrhageData;
import com.kitware.pulse.cdm.bind.PatientActions.IntubationData;
import com.kitware.pulse.cdm.bind.PatientActions.SubstanceBolusData;
import com.kitware.pulse.cdm.bind.PatientActions.SupplementalOxygenData;
import com.kitware.pulse.cdm.bind.PatientAssessments.UrinalysisData.eClarityIndicator;
import com.kitware.pulse.cdm.bind.PatientAssessments.UrinalysisData.ePresenceIndicator;
import com.kitware.pulse.cdm.bind.PatientAssessments.UrinalysisData.eUrineColor;
import com.kitware.pulse.cdm.bind.PatientAssessments.UrinalysisMicroscopicData.eObservationAmount;
import com.kitware.pulse.cdm.bind.PatientAssessments.UrinalysisMicroscopicData.eObservationType;
import com.kitware.pulse.cdm.bind.PatientAssessments.ePatientAssessmentType;
import com.kitware.pulse.cdm.bind.Physiology.eHeartRhythm;
import com.kitware.pulse.cdm.bind.Substance.SubstanceData;
import com.kitware.pulse.cdm.bind.Substance.SubstancePhysicochemicalsData;
import com.kitware.pulse.cdm.compartment.SECompartment;
import com.kitware.pulse.cdm.patient.SEPatient;
import com.kitware.pulse.cdm.patient.actions.SEConsciousRespirationCommand;
import com.kitware.pulse.cdm.patient.actions.SEPatientAction;
import com.kitware.pulse.cdm.patient.assessments.SEPatientAssessment;
import com.kitware.pulse.cdm.patient.conditions.SEPatientCondition;
import com.kitware.pulse.cdm.properties.SEFunction;
import com.kitware.pulse.cdm.properties.SEScalar;
import com.kitware.pulse.cdm.system.environment.actions.SEEnvironmentAction;
import com.kitware.pulse.cdm.system.environment.conditions.SEEnvironmentCondition;
import com.kitware.pulse.cdm.system.equipment.anesthesia_machine.actions.SEAnesthesiaMachineAction;
import com.kitware.pulse.cdm.system.equipment.bag_valve_mask.actions.SEBagValveMaskAction;
import com.kitware.pulse.cdm.system.equipment.ecmo.actions.SEECMOAction;
import com.kitware.pulse.cdm.system.equipment.inhaler.actions.SEInhalerAction;
import com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions.SEMechanicalVentilatorAction;
import com.kitware.pulse.utilities.FileUtils;
import com.kitware.pulse.utilities.FindObjects;
import com.kitware.pulse.utilities.FindObjects.BagMethod;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.StringUtils;
import com.kitware.pulse.utilities.JNIBridge;

/**
 * This class will generate markdown tables
 * for some important cdm data structures
 * for use in our documentation
 */
public class CDM2MD
{
  public static void main(String[] args)
  {
    JNIBridge.initialize();
    // Table names that link to classes (and we don't want them to)
    // For example, tableNameLinks.add("Environment"); will ensure % is prefixed 
    // so doxygen won't hyperlink this word
    // I don't think we have any of these anymore... 
    convert(args.length> 0 ? args[0] : "./docs/markdown");
  }
  
  protected static Set<String> tableNameLinks = new HashSet<>();
  
  public static void convert(String destDir)
  {
    try
    {
      //FileUtils.delete(destDir);//Caller should delete old contents
      FileUtils.createDirectory(destDir);

      PrintWriter writer=new PrintWriter(destDir+"/CDMTable.md", "UTF-8");

      writer.append("CDM Tables {#CDMTables}\n");
      writer.append("=======================\n");

      List<String> skipProperties = new ArrayList<>();
      skipProperties.add("Comment");
      skipProperties.add("ScenarioTime");

      // PATIENT
      writer.append("#### The following tables describe a patient for Pulse to simulate\n<hr>\n");
      WriteDoxyTable(SEPatient.class, "", writer, skipProperties);
      WriteDoxyTable(PatientData.eSex.class, "PatientData_", writer, skipProperties);

      writer.append("#### The following tables describe the physiologic and equipment states Pulse supports.\n<hr>\n");
      WriteDoxyTable(eEvent.class, "", writer, skipProperties);

      // PATIENT CONDITIONS
      writer.append("#### The following tables describe the conditions that can be applied to the patient before starting the simulation\n<hr>\n");
      Set<Class<? extends SEPatientCondition>> pConditions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.patient.conditions", SEPatientCondition.class);
      List<Class<? extends Object>> pConditionsSorted = pConditions.stream().collect(Collectors.toList());
      Collections.sort(pConditionsSorted, (o1, o2) -> o1.getName().compareTo(o2.getName()));
      for(Class<?> c : pConditionsSorted)
        WriteDoxyTable(c, "", writer, skipProperties);

      // PHYSIOLOGY
      writer.append("#### The following tables describe the system data that is calculated each time step\n<hr>\n");
      Set<Class<? extends Object>> phys = FindObjects.findAllClasses("com.kitware.pulse.cdm.system.physiology");
      List<Class<? extends Object>> physSorted = phys.stream().collect(Collectors.toList());
      Collections.sort(physSorted, (o1, o2) -> o1.getName().compareTo(o2.getName()));
      for(Class<?> c : physSorted)
        WriteDoxyTable(c, "", writer, skipProperties);
      WriteDoxyTable(eHeartRhythm.class, "", writer, skipProperties);
      WriteDoxyTable(eBreathState.class, "", writer, skipProperties);
      WriteDoxyTable(eDriverWaveform.class, "", writer, skipProperties);

      // PATIENT ACTIONS/CONDITIONS/ASSESSMENTS
      writer.append("#### The following tables describe the are actions that may be performed on the patient\n<hr>\n");
      Set<Class<? extends SEPatientAction>> pActions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.patient.actions", SEPatientAction.class);
      List<Class<? extends SEPatientAction>> pActionsSorted = pActions.stream().collect(Collectors.toList());
      Collections.sort(pActionsSorted, (o1, o2) -> o1.getName().compareTo(o2.getName()));
      for(Class<?> c : pActionsSorted)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends SEConsciousRespirationCommand>> cmds = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.patient.actions", SEConsciousRespirationCommand.class);
      for(Class<?> c : cmds)
        WriteDoxyTable(c, "", writer, skipProperties);
      WriteDoxyTable(BrainInjuryData.eType.class, "BrainInjuryData_", writer, skipProperties);
      WriteDoxyTable(HemorrhageData.eCompartment.class, "HemorrhageData_", writer, skipProperties);
      WriteDoxyTable(HemorrhageData.eType.class, "HemorrhageData_", writer, skipProperties);
      WriteDoxyTable(IntubationData.eType.class, "IntubationData_", writer, skipProperties);
      WriteDoxyTable(SubstanceBolusData.eRoute.class, "SubstanceBolusData_", writer, skipProperties);
      WriteDoxyTable(SupplementalOxygenData.eDevice.class, "SupplementalOxygenData_", writer, skipProperties);
      WriteDoxyTable(ePatientAssessmentType.class, "", writer, skipProperties);
      Set<Class<? extends Object>> pNutrition = FindObjects.findAllClasses("com.kitware.pulse.cdm.patient.nutrition");
      for(Class<?> c : pNutrition)
        WriteDoxyTable(c, "", writer, skipProperties);

      writer.append("#### The following tables describe the assessments that may be performed on the patient\n<hr>\n");
      Set<Class<? extends SEPatientAssessment>> pAsses = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.patient.assessments", SEPatientAssessment.class);
      for(Class<?> c : pAsses)
        WriteDoxyTable(c, "", writer, skipProperties);
      WriteDoxyTable(ePatientAssessmentType.class, "", writer, skipProperties);
      WriteDoxyTable(ePresenceIndicator.class, "eUrinalysis_", writer, skipProperties);
      WriteDoxyTable(eClarityIndicator.class, "eUrinalysis_", writer, skipProperties);
      WriteDoxyTable(eUrineColor.class, "eUrinalysis_", writer, skipProperties);
      WriteDoxyTable(eObservationType.class, "eUrinalysis_", writer, skipProperties);
      WriteDoxyTable(eObservationAmount.class, "eUrinalysis_", writer, skipProperties);

      // ENVIRONMENT
      writer.append("#### The following tables describe the external environment that surrounds the patient\n<hr>\n");
      Set<Class<? extends Object>> env = FindObjects.findAllClasses("com.kitware.pulse.cdm.system.environment");
      for(Class<?> c : env)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends SEEnvironmentAction>> eActions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.system.environment.actions", SEEnvironmentAction.class);
      for(Class<?> c : eActions)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends SEEnvironmentCondition>> eConditions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.system.environment.conditions", SEEnvironmentCondition.class);
      for(Class<?> c : eConditions)
        WriteDoxyTable(c, "", writer, skipProperties);
      WriteDoxyTable(EnvironmentalConditionsData.eSurroundingType.class, "EnvironmentalConditionsData_", writer, skipProperties);

      // ANESTHESIA MACHINE
      writer.append("#### The following tables describe the anesthesia machine\n<hr>\n");
      Set<Class<? extends Object>> anes = FindObjects.findAllClasses("com.kitware.pulse.cdm.system.equipment.anesthesia_machine");
      for(Class<?> c : anes)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends SEAnesthesiaMachineAction>> aActions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.system.equipment.anesthesia_machine.actions", SEAnesthesiaMachineAction.class);
      for(Class<?> c : aActions)
        WriteDoxyTable(c, "", writer, skipProperties);
      WriteDoxyTable(AnesthesiaMachineData.eOxygenSource.class, "AnesthesiaMachineData_", writer, skipProperties);
      WriteDoxyTable(AnesthesiaMachineData.ePrimaryGas.class, "AnesthesiaMachineData_", writer, skipProperties);

      // BAG VALVE MASK
      writer.append("#### The following tables describe the bag valve mask\n<hr>\n");
      Set<Class<? extends Object>> bvm = FindObjects.findAllClasses("com.kitware.pulse.cdm.system.equipment.bag_valve_mask");
      for(Class<?> c : bvm)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends SEBagValveMaskAction>> bvmActions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.system.equipment.bag_valve_mask.actions", SEBagValveMaskAction.class);
      for(Class<?> c : bvmActions)
        WriteDoxyTable(c, "", writer, skipProperties);
      
      // ECMO
      writer.append("#### The following tables describe the ECMO machine\n<hr>\n");
      Set<Class<? extends Object>> ecmo = FindObjects.findAllClasses("com.kitware.pulse.cdm.system.equipment.ecmo");
      for(Class<?> c : ecmo)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends SEECMOAction>> ecmoActions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.system.equipment.ecmo.actions", SEECMOAction.class);
      for(Class<?> c : ecmoActions)
        WriteDoxyTable(c, "", writer, skipProperties);
      
      // ECG
      writer.append("#### The following tables describe the ECG\n<hr>\n");
      Set<Class<? extends Object>> ecg = FindObjects.findAllClasses("com.kitware.pulse.cdm.system.equipment.electrocardiogram");
      for(Class<?> c : ecg)
        WriteDoxyTable(c, "", writer, skipProperties);

      // INHALER
      writer.append("#### The following tables describe the inhaler\n<hr>\n");
      Set<Class<? extends Object>> inhaler = FindObjects.findAllClasses("com.kitware.pulse.cdm.system.equipment.inhaler");
      for(Class<?> c : inhaler)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends SEInhalerAction>> iActions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.system.equipment.inhaler.actions", SEInhalerAction.class);
      for(Class<?> c : iActions)
        WriteDoxyTable(c, "", writer, skipProperties);

      // MECHANICAL VENTILATOR
      writer.append("#### The following tables describe the mechanical ventilator\n<hr>\n");
      Set<Class<? extends Object>> mech_vent = FindObjects.findAllClasses("com.kitware.pulse.cdm.system.equipment.mechanical_ventilator");
      for(Class<?> c : mech_vent)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends SEMechanicalVentilatorAction>> mvActions = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions", SEMechanicalVentilatorAction.class);
      for(Class<?> c : mvActions)
        WriteDoxyTable(c, "", writer, skipProperties);
      
      // SUBSTSANCE
      writer.append("#### The following tables describe substances used in Pulse\n<hr>\n");
      Set<Class<? extends Object>> subs = FindObjects.findAllClasses("com.kitware.pulse.cdm.substance");
      for(Class<?> c : subs)
        WriteDoxyTable(c, "", writer, skipProperties);
      Set<Class<? extends Object>> subQs = FindObjects.findAllClasses("com.kitware.pulse.cdm.substance.quantity");
      for(Class<?> c : subQs)
        WriteDoxyTable(c, "", writer, skipProperties);
      WriteDoxyTable(SubstanceData.eState.class, "SubstanceData_", writer, skipProperties);
      WriteDoxyTable(SubstancePhysicochemicalsData.eIonicState.class, "SubstancePhysicochemicalsData_", writer, skipProperties);
      WriteDoxyTable(SubstancePhysicochemicalsData.eBindingProtein.class, "SubstancePhysicochemicalsData_", writer, skipProperties);

      // COMPARTMENT
      writer.append("#### The following tables describe anatomical compartments\n<hr>\n");
      Set<Class<? extends SECompartment>> cmpts = FindObjects.findClassSubTypes("com.kitware.pulse.cdm.compartment",SECompartment.class);
      for(Class<?> c : cmpts)
        WriteDoxyTable(c, "", writer, skipProperties);

      // SCENARIO
      writer.append("#### The following tables describe a simulation scenario\n<hr>\n");
      Set<Class<? extends Object>> sce = FindObjects.findAllClasses("com.kitware.pulse.cdm.scenario");
      for(Class<?> c : sce)
        WriteDoxyTable(c, "", writer, skipProperties);

      // General Enums
      writer.append("#### The following tables describe general enumerations\n<hr>\n");
      WriteDoxyTable(eSide.class, "", writer, skipProperties);
      WriteDoxyTable(eGate.class, "", writer, skipProperties);
      WriteDoxyTable(eSwitch.class, "", writer, skipProperties);
      WriteDoxyTable(eCharge.class, "", writer, skipProperties);
      
      writer.close();

    } 
    catch (Exception e)
    {
      Log.error("Could not create directory .markdown");
      Log.error(e.getMessage());
    }
  }

  protected static void WriteDoxyTable(Class<?> c, String prefix, PrintWriter writer, List<String> skipProperties)
  {
    String tableName = c.getSimpleName();
    if(tableName.startsWith("SE"))
      tableName = tableName.substring(2);
    String descPrepend;
    if(c.isEnum())
    {
      tableName = prefix+tableName;
      descPrepend = "@copybrief "+tableName;
    }
    else
      descPrepend = "@copybrief "+prefix+tableName+"Data";
    Log.info("Creating table for "+tableName);

    String columnHeaders[] = new String[3];
    int maxColumnLength[] = new int[columnHeaders.length];
    columnHeaders[0] = "Property Name";
    columnHeaders[1] = "Type";
    columnHeaders[2] = "Definition";
    for(int i=0; i<maxColumnLength.length; i++)
      maxColumnLength[i] = columnHeaders[i].length();

    List<BagMethod> bagMethods = FindObjects.getBagMethods(c,skipProperties);
    if(c.isEnum())
    {
      for(Object o : c.getEnumConstants())
      {
        //Enum<?> e = (Enum<?>)o;
        try
        { 
          if(o.toString().equals("UNRECOGNIZED"))
            continue;
          BagMethod bag = new BagMethod();
          bag.propertyName = o.toString();
          bag.returnType = c;
          bagMethods.add(bag);
        } 
        catch (Exception ex)
        {
          Log.info("Enum is not happy");
          Log.info(ex.getMessage());
        }
      }
    }

    // Compute all of our header values and sizes
    for(BagMethod bag : bagMethods)
    {
      // Header (was built when we had to pull data out of the results file)
      if(bag.propertyName.length()>maxColumnLength[0])
        maxColumnLength[0] = bag.propertyName.length();
      if(bag.returnType.getSimpleName().length()>maxColumnLength[1])
        maxColumnLength[1] = bag.returnType.getSimpleName().length();
    }
    maxColumnLength[2] = descPrepend.length()+maxColumnLength[0];

    try
    {
      // Create file and start the table
      writer.println("");
      writer.println("@anchor "+StringUtils.removeSpaces(tableName)+"Table");
      if(c.isEnum())
        writer.println("##### "+tableName);
      else
      {
        if (tableNameLinks.contains(tableName))
          writer.println("##### %"+tableName);
        else
          writer.println("##### "+tableName);
      }
      writer.println(descPrepend+"");

      if(!bagMethods.isEmpty())
      {
        for(int i=0; i<columnHeaders.length; i++)
          writer.print("|"+pad(columnHeaders[i],maxColumnLength[i]));
        writer.println("|");
        for(int i=0; i<columnHeaders.length; i++)
          writer.print("|"+pad("---",maxColumnLength[i]));
        writer.println("|");
        // Now loop the vData and write out table rows
        for(BagMethod bag : bagMethods)
        {
          writer.print("|"+pad(bag.propertyName,maxColumnLength[0]));
          if(bag.returnType.getSimpleName().equals("List"))
          {
            // If it is a list, I am doing this special code, not generic smart yet
            if(bag.propertyName.equals("AmbientSubstance"))
            {
              writer.print("|"+"List of SubstanceFractions");
              writer.print("|"+"@ref SubstanceFractionTable");
            }
            else if(bag.propertyName.equals("Commands"))
            {
              writer.print("|"+"List of Commands");
              String tables = "@ref ForcedPauseTable <br> @ref ForcedExhaleTable <br> @ref ForcedInhaleTable <br> @ref UseInhalerTable";
              writer.print("|"+tables);
            }
            else if(bag.propertyName.equals("SubstanceQuantities") && tableName.startsWith("Gas"))
            {
              writer.print("|"+"List of SEGasSubstanceQuantity");
              writer.print("|"+"@ref GasSubstanceQuantityTable");
            }
            else if(bag.propertyName.equals("SubstanceQuantities") && tableName.startsWith("Liquid"))
            {
              writer.print("|"+"List of SELiquidSubstanceQuantity");
              writer.print("|"+"@ref LiquidSubstanceQuantityTable");
            }
            else if(bag.propertyName.equals("Components"))
            {
              writer.print("|"+"List of SESubstanceCompoundComponent");
              writer.print("|"+"@ref SubstanceCompoundComponentTable");
            }
            else if(bag.propertyName.equals("GasFraction"))
            {
              writer.print("|"+"List of SESubstanceFraction");
              writer.print("|"+"@ref SubstanceFractionTable");
            }
            else if(bag.propertyName.equals("Aerosol"
                + ""))
            {
              writer.print("|"+"List of SESubstanceConcentration");
              writer.print("|"+"@ref SubstanceConcentrationTable");
            }
            else if(bag.propertyName.equals("AmbientGas"))
            {
              writer.print("|"+"List of SESubstanceFraction");
              writer.print("|"+"@ref SubstanceFractionTable");
            }
            else if(bag.propertyName.equals("AmbientAerosol"))
            {
              writer.print("|"+"List of SESubstanceConcentration");
              writer.print("|"+"@ref SubstanceConcentrationTable");
            }
            else if(bag.propertyName.equals("FractionInspiredGas"))
            {
              writer.print("|"+"List of SESubstanceFraction");
              writer.print("|"+"@ref SubstanceFractionTable");
            }
            else if(bag.propertyName.equals("ConcentrationInspiredAerosol"))
            {
              writer.print("|"+"List of SESubstanceConcentration");
              writer.print("|"+"@ref SubstanceConcentrationTable");
            }
            else if(bag.propertyName.equals("SubstanceConcentration"))
            {
              writer.print("|"+"List of SESubstanceConcentration");
              writer.print("|"+"@ref SubstanceConcentrationTable");
            }
            else if(bag.propertyName.equals("Waveforms"))
            {
              writer.print("|"+"List of SEElectroCardioGramWaveform");
              writer.print("|"+"@ref ElectroCardioGramWaveformTable");
            }
            else
              Log.error("Unsupported List type for :"+bag.propertyName+" on table "+tableName);

          }
          else
          {
            if(Enum.class.isAssignableFrom(bag.returnType))
               writer.print("|"+pad("Enum",maxColumnLength[1]));
            else
              writer.print("|"+pad(bag.returnType.getSimpleName(),maxColumnLength[1]));
            if(SEScalar.class.isAssignableFrom(bag.returnType) || 
                Enum.class.isAssignableFrom(bag.returnType) ||
                String.class.isAssignableFrom(bag.returnType) ||
                SEFunction.class.isAssignableFrom(bag.returnType))
              writer.print("|"+pad(descPrepend+"_"+bag.propertyName,maxColumnLength[2]));
            else
            {
              String refTable = bag.returnType.getSimpleName();
              if(refTable.startsWith("SE"))
                refTable = refTable.substring(2);
              writer.print("|"+pad("@ref "+refTable+"Table",maxColumnLength[2]));
            }
          }
          writer.println("|");
        }   
      }
      writer.println("");
    }
    catch(Exception ex)
    {
      Log.error("Error writing cdm table for "+tableName);
      Log.error(ex.getMessage());
    }
    writer.print("\n<hr>\n");
  }
  protected static String pad(String s, int max)
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

}
