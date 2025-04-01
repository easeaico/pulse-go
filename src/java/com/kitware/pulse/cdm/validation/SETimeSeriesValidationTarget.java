/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.validation;
import java.util.ArrayList;
import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;
import com.kitware.pulse.cdm.bind.Validation.TimeSeriesValidationTargetData;
import com.kitware.pulse.cdm.bind.Validation.TimeSeriesValidationTargetData.eType;
import com.kitware.pulse.cdm.bind.Validation.TimeSeriesValidationTargetListData;
import com.kitware.pulse.utilities.FileUtils;

public class SETimeSeriesValidationTarget extends SEValidationTarget
{
  
  public enum eComparisonType
  {
    TargetEnum, TargetValue, TargetRange
  }
  protected eComparisonType comparisonType;
  protected eType   targetType;
  
  protected String  assessment;
  protected Boolean patientSpecific;
  
  protected String  targetEnum;
  protected double  targetValue;
  protected double  targetMaximum;
  protected double  targetMinimum;
  
  
  public SETimeSeriesValidationTarget()
  {
    super();
  }

  
  public void clear()
  {
    super.clear();
    targetType = eType.Mean;
    comparisonType = eComparisonType.TargetValue;
    
    assessment = "";
    patientSpecific = false;
    
    targetEnum     = "";
    targetValue    = Double.NaN;
    targetMaximum  = Double.NaN;
    targetMinimum  = Double.NaN;
  }
  
  public static List<SETimeSeriesValidationTarget> readFile(String fileName) throws InvalidProtocolBufferException
  {
    TimeSeriesValidationTargetListData.Builder builder = TimeSeriesValidationTargetListData.newBuilder();
    JsonFormat.parser().merge(FileUtils.readFile(fileName), builder);
    List<SETimeSeriesValidationTarget> list = new ArrayList<SETimeSeriesValidationTarget>();
    SETimeSeriesValidationTarget.load(builder.build(), list);
    return list;
  }
  public static void writeFile(List<SETimeSeriesValidationTarget> tgts, String fileName) throws InvalidProtocolBufferException
  {
    FileUtils.writeFile(fileName, JsonFormat.printer().print(SETimeSeriesValidationTarget.unload(tgts)));
  }
  public static void load(TimeSeriesValidationTargetListData src, List<SETimeSeriesValidationTarget> dst)
  {
    for(TimeSeriesValidationTargetData data : src.getTimeSeriesValidationTargetList())
    {
      SETimeSeriesValidationTarget vt = new SETimeSeriesValidationTarget();
      SETimeSeriesValidationTarget.load(data, vt);
    }
  }
  public static TimeSeriesValidationTargetListData unload(List<SETimeSeriesValidationTarget> src)
  {
    TimeSeriesValidationTargetListData.Builder dst = TimeSeriesValidationTargetListData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(List<SETimeSeriesValidationTarget> src, TimeSeriesValidationTargetListData.Builder dst)
  {
    for(SETimeSeriesValidationTarget vt : src)
    {
      dst.addTimeSeriesValidationTarget(SETimeSeriesValidationTarget.unload(vt));
    }
  }
  
  public static void load(TimeSeriesValidationTargetData src, SETimeSeriesValidationTarget dst)
  {
    SEValidationTarget.load(src.getValidationTarget(), dst);
    dst.assessment = src.getAssessment();
    dst.patientSpecific = src.getPatientSpecific();
    
    switch(src.getExpectedCase())
    {
      case TARGETENUM:
        dst.setTargetEnum(src.getTargetEnum());
        break;
      case TARGETVALUE:
        dst.setTargetValue(src.getTargetValue(), src.getType());
        break;
      case TARGETRANGE:
        dst.setTargetRange(
            src.getTargetRange().getMinimum(),
            src.getTargetRange().getMaximum(),
            src.getType());
        break;
      default:
        break;
    }

    dst.targetType = src.getType();
  }
  public static TimeSeriesValidationTargetData unload(SETimeSeriesValidationTarget src)
  {
    TimeSeriesValidationTargetData.Builder dst = TimeSeriesValidationTargetData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SETimeSeriesValidationTarget src, TimeSeriesValidationTargetData.Builder dst)
  {
    SEValidationTarget.unload(src,dst.getValidationTargetBuilder());
    dst.setType(src.targetType);
    dst.setAssessment(src.assessment);
    dst.setPatientSpecific(src.patientSpecific);
    switch(src.comparisonType)
    {
      case TargetEnum:
        dst.setTargetEnum(src.targetEnum);
        break;
      case TargetValue:
        dst.setTargetValue(src.targetValue);
        break;
      case TargetRange:
        dst.getTargetRangeBuilder().setMinimum(src.targetMinimum);
        dst.getTargetRangeBuilder().setMaximum(src.targetMaximum);
        dst.getTargetRangeBuilder().build();
        break;
    }
  }

  public eType getTargetType() { return targetType; }
  
  public void setAssessment(String s) { assessment = s;}
  public String getAssessment() { return assessment; }
  
  public void setPatientSpecific(Boolean b) { patientSpecific = b;}
  public Boolean getPatientSpecific() { return patientSpecific; }
  
  public void setTargetEnum(String s)
  {
    targetEnum = s;
    targetValue = Double.NaN;
    targetMaximum = Double.NaN;
    targetMinimum = Double.NaN;
    targetType = eType.Enumeration;
    comparisonType = eComparisonType.TargetEnum;
  }
  public String getTargetEnum() { return targetEnum; }
  
  public void setTargetValue(double d, eType t)
  {
    targetEnum = "";
    targetValue = d;
    targetMaximum = d;
    targetMinimum = d;
    targetType = t;
    comparisonType = eComparisonType.TargetValue;
  }
  public double getTargetValue() { return targetValue; }
  
  public void setTargetRange(double min, double max, eType t)
  {
    targetEnum = "";
    targetValue = Double.NaN;
    targetMaximum = max;
    targetMinimum = min;
    targetType = t;
    comparisonType = eComparisonType.TargetRange;
  }
  public double getTargetMaximum() { return targetMaximum; }
  public double getTargetMinimum() { return targetMinimum; }

  public eComparisonType getComparisonType() { return comparisonType; }
}
