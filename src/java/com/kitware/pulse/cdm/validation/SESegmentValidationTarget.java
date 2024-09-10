/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.validation;

import com.kitware.pulse.cdm.bind.Validation.SegmentValidationTargetData;

public class SESegmentValidationTarget extends SEValidationTarget
{
  public enum eComparisonType
  {
    EqualToValue, EqualToSegment, GreaterThanValue, GreaterThanSegment,
    LessThanValue, LessThanSegment, TrendToValue, TrendToSegment, Range
  }
  protected eComparisonType m_ComparisonType;
  protected int m_TargetSegment;
  
  public SESegmentValidationTarget()
  {
    super();
  }

  public void clear()
  {
    super.clear();
    m_TargetSegment = 0;
  }

  public static void load(SegmentValidationTargetData src, SESegmentValidationTarget dst)
  {
    SEValidationTarget.load(src.getValidationTarget(), dst);
    switch(src.getExpectedCase())
    {
      case EQUALTOSEGMENT:
        dst.setEqualToSegment(src.getEqualToSegment());
        break;
      case EQUALTOVALUE:
        dst.setEqualToValue(src.getEqualToValue());
        break;
      case GREATERTHANSEGMENT:
        dst.setGreaterThanSegment(src.getGreaterThanSegment());
        break;
      case GREATERTHANVALUE:
        dst.setGreaterThanValue(src.getGreaterThanValue());
        break;
      case LESSTHANSEGMENT:
        dst.setLessThanSegment(src.getLessThanSegment());
        break;
      case LESSTHANVALUE:
        dst.setLessThanValue(src.getLessThanValue());
        break;
      case TRENDSTOSEGMENT:
        dst.setLessThanSegment(src.getLessThanSegment());
        break;
      case TRENDSTOVALUE:
        dst.setLessThanValue(src.getLessThanValue());
        break;
      case RANGE:
        dst.setRange(
            src.getRange().getMinimum(),
            src.getRange().getMaximum());
        break;
      default:
        break;
    }
  }
  public static SegmentValidationTargetData unload(SESegmentValidationTarget src)
  {
    SegmentValidationTargetData.Builder dst = SegmentValidationTargetData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SESegmentValidationTarget src, SegmentValidationTargetData.Builder dst)
  {
    SEValidationTarget.unload(src,dst.getValidationTargetBuilder());
    switch(src.m_ComparisonType)
    {
      case EqualToSegment:
        dst.setEqualToSegment(src.m_TargetSegment);
        break;
      case EqualToValue:
        dst.setEqualToValue(src.m_Target);
        break;
      case GreaterThanSegment:
        dst.setGreaterThanSegment(src.m_TargetSegment);
        break;
      case GreaterThanValue:
        dst.setGreaterThanValue(src.m_Target);
        break;
      case LessThanSegment:
        dst.setLessThanSegment(src.m_TargetSegment);
        break;
      case LessThanValue:
        dst.setLessThanValue(src.m_Target);
        break;
      case TrendToSegment:
        dst.setTrendsToSegment(src.m_TargetSegment);
        break;
      case TrendToValue:
        dst.setTrendsToValue(src.m_Target);
        break;
      case Range:
        dst.getRangeBuilder().setMinimum(src.m_TargetMinimum);
        dst.getRangeBuilder().setMaximum(src.m_TargetMaximum);
        dst.getRangeBuilder().build();
        break;
    }
  }

  public int GetTargetSegment() { return m_TargetSegment; }

  public void setEqualToSegment(int s)
  {
    m_Target = Double.NaN;
    m_TargetMaximum = Double.NaN;
    m_TargetMinimum = Double.NaN;
    m_TargetSegment = s;
    m_ComparisonType = eComparisonType.EqualToSegment;
  }
  public void setEqualToValue(double d)
  {
    m_Target = d;
    m_TargetMaximum = d;
    m_TargetMinimum = d;
    m_TargetSegment = 0;
    m_ComparisonType = eComparisonType.EqualToValue;
  }
  public void setGreaterThanSegment(int s)
  {
    m_Target = Double.NaN;
    m_TargetMaximum = Double.NaN;
    m_TargetMinimum = Double.NaN;
    m_TargetSegment = s;
    m_ComparisonType = eComparisonType.GreaterThanSegment;
    
  }
  public void setGreaterThanValue(double d)
  {
    m_Target = d;
    m_TargetMaximum = d;
    m_TargetMinimum = d;
    m_TargetSegment = 0;
    m_ComparisonType = eComparisonType.GreaterThanValue;
    
  }
  public void setLessThanSegment(int s)
  {
    m_Target = Double.NaN;
    m_TargetMaximum = Double.NaN;
    m_TargetMinimum = Double.NaN;
    m_TargetSegment = s;
    m_ComparisonType = eComparisonType.LessThanSegment;
  }
  public void setLessThanValue(double d)
  {
    m_Target = d;
    m_TargetMaximum = d;
    m_TargetMinimum = d;
    m_TargetSegment = 0;
    m_ComparisonType = eComparisonType.LessThanValue;
  }
  public void setTrendsToSegment(int s)
  {
    m_Target = Double.NaN;
    m_TargetMaximum = Double.NaN;
    m_TargetMinimum = Double.NaN;
    m_TargetSegment = s;
    m_ComparisonType = eComparisonType.TrendToSegment;
  }
  public void setTrendsToValue(double d)
  {
    m_Target = d;
    m_TargetMaximum = d;
    m_TargetMinimum = d;
    m_TargetSegment = 0;
    m_ComparisonType = eComparisonType.TrendToValue;
  }
  public void setRange(double min, double max)
  {
    m_Target = Double.NaN;
    m_TargetMaximum = max;
    m_TargetMinimum = min;
    m_TargetSegment = 0;
    m_ComparisonType = eComparisonType.Range;
  }
  public eComparisonType getComparisonType() { return m_ComparisonType; }
}
