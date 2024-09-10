/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/Properties.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/properties/SECurve.h"

#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarAmount.h"
#include "cdm/properties/SEScalarAmountPerMass.h"
#include "cdm/properties/SEScalarAmountPerTime.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarAreaPerTimePressure.h"
#include "cdm/properties/SEScalarElectricCapacitance.h"
#include "cdm/properties/SEScalarElectricCharge.h"
#include "cdm/properties/SEScalarElectricCurrent.h"
#include "cdm/properties/SEScalarElectricInductance.h"
#include "cdm/properties/SEScalarElectricPotential.h"
#include "cdm/properties/SEScalarElectricResistance.h"
#include "cdm/properties/SEScalarEnergy.h"
#include "cdm/properties/SEScalarEnergyPerAmount.h"
#include "cdm/properties/SEScalarEnergyPerMass.h"
#include "cdm/properties/SEScalarEquivalentWeightPerVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimeSquaredPerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarForce.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarHeatCapacitance.h"
#include "cdm/properties/SEScalarHeatCapacitancePerAmount.h"
#include "cdm/properties/SEScalarHeatCapacitancePerMass.h"
#include "cdm/properties/SEScalarHeatConductance.h"
#include "cdm/properties/SEScalarHeatConductancePerArea.h"
#include "cdm/properties/SEScalarHeatInductance.h"
#include "cdm/properties/SEScalarHeatResistance.h"
#include "cdm/properties/SEScalarHeatResistanceArea.h"
#include "cdm/properties/SEScalarInversePressure.h"
#include "cdm/properties/SEScalarInverseVolume.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarLengthPerTime.h"
#include "cdm/properties/SEScalarLengthPerTimePressure.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerAmount.h"
#include "cdm/properties/SEScalarMassPerAreaTime.h"
#include "cdm/properties/SEScalarMassPerMass.h"
#include "cdm/properties/SEScalarMassPerTime.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarNegative1To1.h"
#include "cdm/properties/SEScalarOsmolality.h"
#include "cdm/properties/SEScalarOsmolarity.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarPowerPerAreaTemperatureToTheFourth.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerArea.h"
#include "cdm/properties/SEScalarPressureTimePerVolumeArea.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarUnsigned.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerTimeArea.h"
#include "cdm/properties/SEScalarVolumePerTimeMass.h"
#include "cdm/properties/SEScalarVolumePerTimePressure.h"
#include "cdm/properties/SEScalarVolumePerTimePressureArea.h"

#include "cdm/properties/SEArrayElectricPotential.h"

#include "cdm/properties/SEFunctionElectricPotentialVsTime.h"
#include "cdm/properties/SEFunctionVolumeVsTime.h"
#include "cdm/properties/SEHistogramFractionVsLength.h"

#include "cdm/properties/SERunningAverage.h"
#include "cdm/properties/SETemporalInterpolator.h"

void PBProperty::Copy(const SECurve& src, SECurve& dst)
{
  dst.Invalidate();
  CDM_BIND::CurveData data;
  PBProperty::Serialize(src, data);
  PBProperty::Serialize(data, dst);
}
void PBProperty::Load(const CDM_BIND::CurveData& src, SECurve& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::CurveData& src, SECurve& dst)
{
  for (auto s = 0; s<src.segment_size(); s++)
  {
    const CDM_BIND::AnySegmentData& sData = src.segment()[s];
    if (sData.has_constantsegment())
    {
      PBProperty::Load(sData.constantsegment(), dst.AddConstantSegment());
      continue;
    }
    if (sData.has_linearsegment())
    {
      PBProperty::Load(sData.linearsegment(), dst.AddLinearSegment());
      continue;
    }
    if (sData.has_parabolicsegment())
    {
      PBProperty::Load(sData.parabolicsegment(), dst.AddParabolicSegment());
      continue;
    }
    if (sData.has_sigmoidalsegment())
    {
      PBProperty::Load(sData.sigmoidalsegment(), dst.AddSigmoidalSegment());
      continue;
    }
  }
}
CDM_BIND::CurveData* PBProperty::Unload(const SECurve& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::CurveData* dst = new CDM_BIND::CurveData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SECurve& src, CDM_BIND::CurveData& dst)
{
  for (const SESegment* seg : src.GetSegments())
  {
    const SESegmentConstant* c = dynamic_cast<const SESegmentConstant*>(seg);
    if (c != nullptr)
    {
      PBProperty::Serialize(*c, *dst.add_segment()->mutable_constantsegment());
      continue;
    }
    const SESegmentLinear* l = dynamic_cast<const SESegmentLinear*>(seg);
    if (l != nullptr)
    {
      PBProperty::Serialize(*l, *dst.add_segment()->mutable_linearsegment());
      continue;
    }
    const SESegmentParabolic* p = dynamic_cast<const SESegmentParabolic*>(seg);
    if (p != nullptr)
    {
      PBProperty::Serialize(*p, *dst.add_segment()->mutable_parabolicsegment());
      continue;
    }
    const SESegmentSigmoidal* s = dynamic_cast<const SESegmentSigmoidal*>(seg);
    if (s != nullptr)
    {
      PBProperty::Serialize(*s, *dst.add_segment()->mutable_sigmoidalsegment());
      continue;
    }
  }
}

void PBProperty::Load(const CDM_BIND::SegmentConstantData& src, SESegmentConstant& dst)
{
  dst.Clear();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::SegmentConstantData& src, SESegmentConstant& dst)
{
  if (src.has_beginvolume())
    PBProperty::Load(src.beginvolume(), dst.GetBeginVolume());
  if (src.has_endvolume())
    PBProperty::Load(src.endvolume(), dst.GetEndVolume());
  if (src.has_compliance())
    PBProperty::Load(src.compliance(), dst.GetCompliance());
}
CDM_BIND::SegmentConstantData* PBProperty::Unload(const SESegmentConstant& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::SegmentConstantData* dst = new CDM_BIND::SegmentConstantData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SESegmentConstant& src, CDM_BIND::SegmentConstantData& dst)
{
  if (src.HasBeginVolume())
    dst.set_allocated_beginvolume(PBProperty::Unload(*src.m_BeginVolume));
  if (src.HasEndVolume())
    dst.set_allocated_endvolume(PBProperty::Unload(*src.m_EndVolume));
  if (src.HasCompliance())
    dst.set_allocated_compliance(PBProperty::Unload(*src.m_Compliance));
}

void PBProperty::Load(const CDM_BIND::SegmentLinearData& src, SESegmentLinear& dst)
{
  dst.Clear();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::SegmentLinearData& src, SESegmentLinear& dst)
{
  if (src.has_beginvolume())
    PBProperty::Load(src.beginvolume(), dst.GetBeginVolume());
  if (src.has_endvolume())
    PBProperty::Load(src.endvolume(), dst.GetEndVolume());
  if (src.has_slope())
    PBProperty::Load(src.slope(), dst.GetSlope());
  if (src.has_yintercept())
    PBProperty::Load(src.yintercept(), dst.GetYIntercept());
}
CDM_BIND::SegmentLinearData* PBProperty::Unload(const SESegmentLinear& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::SegmentLinearData* dst = new CDM_BIND::SegmentLinearData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SESegmentLinear& src, CDM_BIND::SegmentLinearData& dst)
{
  if (src.HasBeginVolume())
    dst.set_allocated_beginvolume(PBProperty::Unload(*src.m_BeginVolume));
  if (src.HasEndVolume())
    dst.set_allocated_endvolume(PBProperty::Unload(*src.m_EndVolume));
  if (src.HasSlope())
    dst.set_allocated_slope(PBProperty::Unload(*src.m_Slope));
  if (src.HasYIntercept())
    dst.set_allocated_yintercept(PBProperty::Unload(*src.m_YIntercept));
}

void PBProperty::Load(const CDM_BIND::SegmentParabolicData& src, SESegmentParabolic& dst)
{
  dst.Clear();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::SegmentParabolicData& src, SESegmentParabolic& dst)
{
  if (src.has_beginvolume())
    PBProperty::Load(src.beginvolume(), dst.GetBeginVolume());
  if (src.has_endvolume())
    PBProperty::Load(src.endvolume(), dst.GetEndVolume());
  if (src.has_coefficient1())
    PBProperty::Load(src.coefficient1(), dst.GetCoefficient1());
  if (src.has_coefficient2())
    PBProperty::Load(src.coefficient2(), dst.GetCoefficient2());
  if (src.has_coefficient3())
    PBProperty::Load(src.coefficient3(), dst.GetCoefficient3());
  if (src.has_coefficient4())
    PBProperty::Load(src.coefficient4(), dst.GetCoefficient4());
}
CDM_BIND::SegmentParabolicData* PBProperty::Unload(const SESegmentParabolic& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::SegmentParabolicData* dst = new CDM_BIND::SegmentParabolicData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SESegmentParabolic& src, CDM_BIND::SegmentParabolicData& dst)
{
  if (src.HasBeginVolume())
    dst.set_allocated_beginvolume(PBProperty::Unload(*src.m_BeginVolume));
  if (src.HasEndVolume())
    dst.set_allocated_endvolume(PBProperty::Unload(*src.m_EndVolume));
  if (src.HasCoefficient1())
    dst.set_allocated_coefficient1(PBProperty::Unload(*src.m_Coefficient1));
  if (src.HasCoefficient2())
    dst.set_allocated_coefficient2(PBProperty::Unload(*src.m_Coefficient2));
  if (src.HasCoefficient3())
    dst.set_allocated_coefficient3(PBProperty::Unload(*src.m_Coefficient3));
  if (src.HasCoefficient4())
    dst.set_allocated_coefficient4(PBProperty::Unload(*src.m_Coefficient4));
}

void PBProperty::Load(const CDM_BIND::SegmentSigmoidalData& src, SESegmentSigmoidal& dst)
{
  dst.Clear();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::SegmentSigmoidalData& src, SESegmentSigmoidal& dst)
{
  if (src.has_beginvolume())
    PBProperty::Load(src.beginvolume(), dst.GetBeginVolume());
  if (src.has_endvolume())
    PBProperty::Load(src.endvolume(), dst.GetEndVolume());
  if (src.has_lowercorner())
    PBProperty::Load(src.lowercorner(), dst.GetLowerCorner());
  if (src.has_uppercorner())
    PBProperty::Load(src.uppercorner(), dst.GetUpperCorner());
  if (src.has_baselinecompliance())
    PBProperty::Load(src.baselinecompliance(), dst.GetBaselineCompliance());
}
CDM_BIND::SegmentSigmoidalData* PBProperty::Unload(const SESegmentSigmoidal& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::SegmentSigmoidalData* dst = new CDM_BIND::SegmentSigmoidalData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SESegmentSigmoidal& src, CDM_BIND::SegmentSigmoidalData& dst)
{
  if (src.HasBeginVolume())
    dst.set_allocated_beginvolume(PBProperty::Unload(*src.m_BeginVolume));
  if (src.HasEndVolume())
    dst.set_allocated_endvolume(PBProperty::Unload(*src.m_EndVolume));
  if (src.HasUpperCorner())
    dst.set_allocated_uppercorner(PBProperty::Unload(*src.m_UpperCorner));
  if (src.HasLowerCorner())
    dst.set_allocated_lowercorner(PBProperty::Unload(*src.m_LowerCorner));
  if (src.HasBaselineCompliance())
    dst.set_allocated_baselinecompliance(PBProperty::Unload(*src.m_BaselineCompliance));
}

void PBProperty::Load(const CDM_BIND::ScalarData& src, SEScalar& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarData& src, SEScalar& dst)
{
  dst.SetValue(src.value());
  if (!src.unit().empty())
  {
    if (src.unit() != "unitless")
      throw CommonDataModelException("CDM_BIND::Scalar API is intended to be unitless, You are trying to load a ScalarData with a unit defined");
  }
  dst.m_readOnly = src.readonly();
}
CDM_BIND::ScalarData* PBProperty::Unload(const SEScalar& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarData* dst = new CDM_BIND::ScalarData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalar& src, CDM_BIND::ScalarData& dst)
{
  dst.set_value(src.m_value);
  dst.set_readonly(src.m_readOnly);
}
void PBProperty::Serialize(const CDM_BIND::ScalarData& src, SEUnitScalar& dst)
{
  if (!src.unit().empty())
    dst.SetValue(src.value(), *dst.GetCompoundUnit(src.unit()));
  else
    throw CommonDataModelException("ScalarQuantity attempted to load a ScalarData with no unit, must have a unit.");
  dst.m_readOnly = src.readonly();
}
void PBProperty::Serialize(const SEUnitScalar& src, CDM_BIND::ScalarData& dst)
{
  dst.set_value(src.m_value);
  if (src.HasUnit())
    dst.set_unit(src.GetUnit()->GetString());
  else
    throw CommonDataModelException("ScalarQuantity attempted to unload a ScalarData with no unit, must have a unit.");
  dst.set_readonly(src.m_readOnly);
}


void PBProperty::Load(const CDM_BIND::Scalar0To1Data& src, SEScalar0To1& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::Scalar0To1Data& src, SEScalar0To1& dst)
{
  PBProperty::Serialize(src.scalar0to1(), dst);
}
CDM_BIND::Scalar0To1Data* PBProperty::Unload(const SEScalar0To1& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::Scalar0To1Data* dst = new CDM_BIND::Scalar0To1Data();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalar0To1& src, CDM_BIND::Scalar0To1Data& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalar0to1());
}

void PBProperty::Load(const CDM_BIND::ScalarAmountData& src, SEScalarAmount& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarAmountData& src, SEScalarAmount& dst)
{
  PBProperty::Serialize(src.scalaramount(), dst);
}
CDM_BIND::ScalarAmountData* PBProperty::Unload(const SEScalarAmount& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarAmountData* dst = new CDM_BIND::ScalarAmountData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarAmount& src, CDM_BIND::ScalarAmountData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalaramount());
}


void PBProperty::Load(const CDM_BIND::ScalarAmountPerMassData& src, SEScalarAmountPerMass& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarAmountPerMassData& src, SEScalarAmountPerMass& dst)
{
  PBProperty::Serialize(src.scalaramountpermass(), dst);
}
CDM_BIND::ScalarAmountPerMassData* PBProperty::Unload(const SEScalarAmountPerMass& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarAmountPerMassData* dst = new CDM_BIND::ScalarAmountPerMassData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarAmountPerMass& src, CDM_BIND::ScalarAmountPerMassData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalaramountpermass());
}

void PBProperty::Load(const CDM_BIND::ScalarAmountPerTimeData& src, SEScalarAmountPerTime& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarAmountPerTimeData& src, SEScalarAmountPerTime& dst)
{
  PBProperty::Serialize(src.scalaramountpertime(), dst);
}
CDM_BIND::ScalarAmountPerTimeData* PBProperty::Unload(const SEScalarAmountPerTime& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarAmountPerTimeData* dst = new CDM_BIND::ScalarAmountPerTimeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarAmountPerTime& src, CDM_BIND::ScalarAmountPerTimeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalaramountpertime());
}

void PBProperty::Load(const CDM_BIND::ScalarAmountPerVolumeData& src, SEScalarAmountPerVolume& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarAmountPerVolumeData& src, SEScalarAmountPerVolume& dst)
{
  PBProperty::Serialize(src.scalaramountpervolume(), dst);
}
CDM_BIND::ScalarAmountPerVolumeData* PBProperty::Unload(const SEScalarAmountPerVolume& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarAmountPerVolumeData* dst = new CDM_BIND::ScalarAmountPerVolumeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarAmountPerVolume& src, CDM_BIND::ScalarAmountPerVolumeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalaramountpervolume());
}

void PBProperty::Load(const CDM_BIND::ScalarAreaData& src, SEScalarArea& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarAreaData& src, SEScalarArea& dst)
{
  PBProperty::Serialize(src.scalararea(), dst);
}
CDM_BIND::ScalarAreaData* PBProperty::Unload(const SEScalarArea& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarAreaData* dst = new CDM_BIND::ScalarAreaData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarArea& src, CDM_BIND::ScalarAreaData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalararea());
}

void PBProperty::Load(const CDM_BIND::ScalarAreaPerTimePressureData& src, SEScalarAreaPerTimePressure& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarAreaPerTimePressureData& src, SEScalarAreaPerTimePressure& dst)
{
  PBProperty::Serialize(src.scalarareapertimepressure(), dst);
}
CDM_BIND::ScalarAreaPerTimePressureData* PBProperty::Unload(const SEScalarAreaPerTimePressure& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarAreaPerTimePressureData* dst = new CDM_BIND::ScalarAreaPerTimePressureData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarAreaPerTimePressure& src, CDM_BIND::ScalarAreaPerTimePressureData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarareapertimepressure());
}

void PBProperty::Load(const CDM_BIND::ScalarElectricCapacitanceData& src, SEScalarElectricCapacitance& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarElectricCapacitanceData& src, SEScalarElectricCapacitance& dst)
{
  PBProperty::Serialize(src.scalarelectriccapacitance(), dst);
}
CDM_BIND::ScalarElectricCapacitanceData* PBProperty::Unload(const SEScalarElectricCapacitance& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarElectricCapacitanceData* dst = new CDM_BIND::ScalarElectricCapacitanceData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarElectricCapacitance& src, CDM_BIND::ScalarElectricCapacitanceData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarelectriccapacitance());
}

void PBProperty::Load(const CDM_BIND::ScalarElectricChargeData& src, SEScalarElectricCharge& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarElectricChargeData& src, SEScalarElectricCharge& dst)
{
  PBProperty::Serialize(src.scalarelectriccharge(), dst);
}
CDM_BIND::ScalarElectricChargeData* PBProperty::Unload(const SEScalarElectricCharge& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarElectricChargeData* dst = new CDM_BIND::ScalarElectricChargeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarElectricCharge& src, CDM_BIND::ScalarElectricChargeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarelectriccharge());
}

void PBProperty::Load(const CDM_BIND::ScalarElectricCurrentData& src, SEScalarElectricCurrent& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarElectricCurrentData& src, SEScalarElectricCurrent& dst)
{
  PBProperty::Serialize(src.scalarelectriccurrent(), dst);
}
CDM_BIND::ScalarElectricCurrentData* PBProperty::Unload(const SEScalarElectricCurrent& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarElectricCurrentData* dst = new CDM_BIND::ScalarElectricCurrentData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarElectricCurrent& src, CDM_BIND::ScalarElectricCurrentData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarelectriccurrent());
}

void PBProperty::Load(const CDM_BIND::ScalarElectricInductanceData& src, SEScalarElectricInductance& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarElectricInductanceData& src, SEScalarElectricInductance& dst)
{
  PBProperty::Serialize(src.scalarelectricinductance(), dst);
}
CDM_BIND::ScalarElectricInductanceData* PBProperty::Unload(const SEScalarElectricInductance& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarElectricInductanceData* dst = new CDM_BIND::ScalarElectricInductanceData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarElectricInductance& src, CDM_BIND::ScalarElectricInductanceData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarelectricinductance());
}

void PBProperty::Load(const CDM_BIND::ScalarElectricPotentialData& src, SEScalarElectricPotential& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarElectricPotentialData& src, SEScalarElectricPotential& dst)
{
  PBProperty::Serialize(src.scalarelectricpotential(), dst);
}
CDM_BIND::ScalarElectricPotentialData* PBProperty::Unload(const SEScalarElectricPotential& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarElectricPotentialData* dst = new CDM_BIND::ScalarElectricPotentialData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarElectricPotential& src, CDM_BIND::ScalarElectricPotentialData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarelectricpotential());
}

void PBProperty::Load(const CDM_BIND::ScalarElectricResistanceData& src, SEScalarElectricResistance& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarElectricResistanceData& src, SEScalarElectricResistance& dst)
{
  PBProperty::Serialize(src.scalarelectricresistance(), dst);
}
CDM_BIND::ScalarElectricResistanceData* PBProperty::Unload(const SEScalarElectricResistance& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarElectricResistanceData* dst = new CDM_BIND::ScalarElectricResistanceData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarElectricResistance& src, CDM_BIND::ScalarElectricResistanceData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarelectricresistance());
}

void PBProperty::Load(const CDM_BIND::ScalarEnergyData& src, SEScalarEnergy& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarEnergyData& src, SEScalarEnergy& dst)
{
  PBProperty::Serialize(src.scalarenergy(), dst);
}
CDM_BIND::ScalarEnergyData* PBProperty::Unload(const SEScalarEnergy& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarEnergyData* dst = new CDM_BIND::ScalarEnergyData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarEnergy& src, CDM_BIND::ScalarEnergyData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarenergy());
}

void PBProperty::Load(const CDM_BIND::ScalarEnergyPerAmountData& src, SEScalarEnergyPerAmount& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarEnergyPerAmountData& src, SEScalarEnergyPerAmount& dst)
{
  PBProperty::Serialize(src.scalarenergyperamount(), dst);
}
CDM_BIND::ScalarEnergyPerAmountData* PBProperty::Unload(const SEScalarEnergyPerAmount& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarEnergyPerAmountData* dst = new CDM_BIND::ScalarEnergyPerAmountData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarEnergyPerAmount& src, CDM_BIND::ScalarEnergyPerAmountData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarenergyperamount());
}

void PBProperty::Load(const CDM_BIND::ScalarEnergyPerMassData& src, SEScalarEnergyPerMass& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarEnergyPerMassData& src, SEScalarEnergyPerMass& dst)
{
  PBProperty::Serialize(src.scalarenergypermass(), dst);
}
CDM_BIND::ScalarEnergyPerMassData* PBProperty::Unload(const SEScalarEnergyPerMass& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarEnergyPerMassData* dst = new CDM_BIND::ScalarEnergyPerMassData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarEnergyPerMass& src, CDM_BIND::ScalarEnergyPerMassData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarenergypermass());
}

void PBProperty::Load(const CDM_BIND::ScalarEquivalentWeightPerVolumeData& src, SEScalarEquivalentWeightPerVolume& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarEquivalentWeightPerVolumeData& src, SEScalarEquivalentWeightPerVolume& dst)
{
  PBProperty::Serialize(src.scalarequivalentweightpervolume(), dst);
}
CDM_BIND::ScalarEquivalentWeightPerVolumeData* PBProperty::Unload(const SEScalarEquivalentWeightPerVolume& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarEquivalentWeightPerVolumeData* dst = new CDM_BIND::ScalarEquivalentWeightPerVolumeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarEquivalentWeightPerVolume& src, CDM_BIND::ScalarEquivalentWeightPerVolumeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarequivalentweightpervolume());
}

void PBProperty::Load(const CDM_BIND::ScalarForceData& src, SEScalarForce& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarForceData& src, SEScalarForce& dst)
{
  PBProperty::Serialize(src.scalarforce(), dst);
}
CDM_BIND::ScalarForceData* PBProperty::Unload(const SEScalarForce& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarForceData* dst = new CDM_BIND::ScalarForceData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarForce& src, CDM_BIND::ScalarForceData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarforce());
}

void PBProperty::Load(const CDM_BIND::ScalarFrequencyData& src, SEScalarFrequency& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarFrequencyData& src, SEScalarFrequency& dst)
{
  PBProperty::Serialize(src.scalarfrequency(), dst);
}
CDM_BIND::ScalarFrequencyData* PBProperty::Unload(const SEScalarFrequency& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarFrequencyData* dst = new CDM_BIND::ScalarFrequencyData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarFrequency& src, CDM_BIND::ScalarFrequencyData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarfrequency());
}

void PBProperty::Load(const CDM_BIND::ScalarHeatCapacitanceData& src, SEScalarHeatCapacitance& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarHeatCapacitanceData& src, SEScalarHeatCapacitance& dst)
{
  PBProperty::Serialize(src.scalarheatcapacitance(), dst);
}
CDM_BIND::ScalarHeatCapacitanceData* PBProperty::Unload(const SEScalarHeatCapacitance& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarHeatCapacitanceData* dst = new CDM_BIND::ScalarHeatCapacitanceData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarHeatCapacitance& src, CDM_BIND::ScalarHeatCapacitanceData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarheatcapacitance());
}

void PBProperty::Load(const CDM_BIND::ScalarHeatCapacitancePerAmountData& src, SEScalarHeatCapacitancePerAmount& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarHeatCapacitancePerAmountData& src, SEScalarHeatCapacitancePerAmount& dst)
{
  PBProperty::Serialize(src.scalarheatcapacitanceperamount(), dst);
}
CDM_BIND::ScalarHeatCapacitancePerAmountData* PBProperty::Unload(const SEScalarHeatCapacitancePerAmount& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarHeatCapacitancePerAmountData* dst = new CDM_BIND::ScalarHeatCapacitancePerAmountData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarHeatCapacitancePerAmount& src, CDM_BIND::ScalarHeatCapacitancePerAmountData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarheatcapacitanceperamount());
}

void PBProperty::Load(const CDM_BIND::ScalarHeatCapacitancePerMassData& src, SEScalarHeatCapacitancePerMass& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarHeatCapacitancePerMassData& src, SEScalarHeatCapacitancePerMass& dst)
{
  PBProperty::Serialize(src.scalarheatcapacitancepermass(), dst);
}
CDM_BIND::ScalarHeatCapacitancePerMassData* PBProperty::Unload(const SEScalarHeatCapacitancePerMass& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarHeatCapacitancePerMassData* dst = new CDM_BIND::ScalarHeatCapacitancePerMassData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarHeatCapacitancePerMass& src, CDM_BIND::ScalarHeatCapacitancePerMassData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarheatcapacitancepermass());
}

void PBProperty::Load(const CDM_BIND::ScalarHeatConductanceData& src, SEScalarHeatConductance& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarHeatConductanceData& src, SEScalarHeatConductance& dst)
{
  PBProperty::Serialize(src.scalarheatconductance(), dst);
}
CDM_BIND::ScalarHeatConductanceData* PBProperty::Unload(const SEScalarHeatConductance& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarHeatConductanceData* dst = new CDM_BIND::ScalarHeatConductanceData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarHeatConductance& src, CDM_BIND::ScalarHeatConductanceData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarheatconductance());
}

void PBProperty::Load(const CDM_BIND::ScalarHeatConductancePerAreaData& src, SEScalarHeatConductancePerArea& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarHeatConductancePerAreaData& src, SEScalarHeatConductancePerArea& dst)
{
  PBProperty::Serialize(src.scalarheatconductanceperarea(), dst);
}
CDM_BIND::ScalarHeatConductancePerAreaData* PBProperty::Unload(const SEScalarHeatConductancePerArea& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarHeatConductancePerAreaData* dst = new CDM_BIND::ScalarHeatConductancePerAreaData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarHeatConductancePerArea& src, CDM_BIND::ScalarHeatConductancePerAreaData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarheatconductanceperarea());
}

void PBProperty::Load(const CDM_BIND::ScalarHeatInductanceData& src, SEScalarHeatInductance& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarHeatInductanceData& src, SEScalarHeatInductance& dst)
{
  PBProperty::Serialize(src.scalarheatinductance(), dst);
}
CDM_BIND::ScalarHeatInductanceData* PBProperty::Unload(const SEScalarHeatInductance& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarHeatInductanceData* dst = new CDM_BIND::ScalarHeatInductanceData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarHeatInductance& src, CDM_BIND::ScalarHeatInductanceData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarheatinductance());
}

void PBProperty::Load(const CDM_BIND::ScalarHeatResistanceData& src, SEScalarHeatResistance& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarHeatResistanceData& src, SEScalarHeatResistance& dst)
{
  PBProperty::Serialize(src.scalarheatresistance(), dst);
}
CDM_BIND::ScalarHeatResistanceData* PBProperty::Unload(const SEScalarHeatResistance& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarHeatResistanceData* dst = new CDM_BIND::ScalarHeatResistanceData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarHeatResistance& src, CDM_BIND::ScalarHeatResistanceData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarheatresistance());
}

void PBProperty::Load(const CDM_BIND::ScalarHeatResistanceAreaData& src, SEScalarHeatResistanceArea& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarHeatResistanceAreaData& src, SEScalarHeatResistanceArea& dst)
{
  PBProperty::Serialize(src.scalarheatresistancearea(), dst);
}
CDM_BIND::ScalarHeatResistanceAreaData* PBProperty::Unload(const SEScalarHeatResistanceArea& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarHeatResistanceAreaData* dst = new CDM_BIND::ScalarHeatResistanceAreaData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarHeatResistanceArea& src, CDM_BIND::ScalarHeatResistanceAreaData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarheatresistancearea());
}

void PBProperty::Load(const CDM_BIND::ScalarInversePressureData& src, SEScalarInversePressure& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarInversePressureData& src, SEScalarInversePressure& dst)
{
  PBProperty::Serialize(src.scalarinversepressure(), dst);
}
CDM_BIND::ScalarInversePressureData* PBProperty::Unload(const SEScalarInversePressure& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarInversePressureData* dst = new CDM_BIND::ScalarInversePressureData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarInversePressure& src, CDM_BIND::ScalarInversePressureData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarinversepressure());
}

void PBProperty::Load(const CDM_BIND::ScalarInverseVolumeData& src, SEScalarInverseVolume& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarInverseVolumeData& src, SEScalarInverseVolume& dst)
{
  PBProperty::Serialize(src.scalarinversevolume(), dst);
}
CDM_BIND::ScalarInverseVolumeData* PBProperty::Unload(const SEScalarInverseVolume& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarInverseVolumeData* dst = new CDM_BIND::ScalarInverseVolumeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarInverseVolume& src, CDM_BIND::ScalarInverseVolumeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarinversevolume());
}

void PBProperty::Load(const CDM_BIND::ScalarLengthData& src, SEScalarLength& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarLengthData& src, SEScalarLength& dst)
{
  PBProperty::Serialize(src.scalarlength(), dst);
}
CDM_BIND::ScalarLengthData* PBProperty::Unload(const SEScalarLength& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarLengthData* dst = new CDM_BIND::ScalarLengthData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarLength& src, CDM_BIND::ScalarLengthData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarlength());
}

void PBProperty::Load(const CDM_BIND::ScalarLengthPerTimeData& src, SEScalarLengthPerTime& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarLengthPerTimeData& src, SEScalarLengthPerTime& dst)
{
  PBProperty::Serialize(src.scalarlengthpertime(), dst);
}
CDM_BIND::ScalarLengthPerTimeData* PBProperty::Unload(const SEScalarLengthPerTime& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarLengthPerTimeData* dst = new CDM_BIND::ScalarLengthPerTimeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarLengthPerTime& src, CDM_BIND::ScalarLengthPerTimeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarlengthpertime());
}

void PBProperty::Load(const CDM_BIND::ScalarLengthPerTimePressureData& src, SEScalarLengthPerTimePressure& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarLengthPerTimePressureData& src, SEScalarLengthPerTimePressure& dst)
{
  PBProperty::Serialize(src.scalarlengthpertimepressure(), dst);
}
CDM_BIND::ScalarLengthPerTimePressureData* PBProperty::Unload(const SEScalarLengthPerTimePressure& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarLengthPerTimePressureData* dst = new CDM_BIND::ScalarLengthPerTimePressureData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarLengthPerTimePressure& src, CDM_BIND::ScalarLengthPerTimePressureData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarlengthpertimepressure());
}

void PBProperty::Load(const CDM_BIND::ScalarMassData& src, SEScalarMass& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarMassData& src, SEScalarMass& dst)
{
  PBProperty::Serialize(src.scalarmass(), dst);
}
CDM_BIND::ScalarMassData* PBProperty::Unload(const SEScalarMass& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarMassData* dst = new CDM_BIND::ScalarMassData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarMass& src, CDM_BIND::ScalarMassData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarmass());
}

void PBProperty::Load(const CDM_BIND::ScalarMassPerAmountData& src, SEScalarMassPerAmount& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarMassPerAmountData& src, SEScalarMassPerAmount& dst)
{
  PBProperty::Serialize(src.scalarmassperamount(), dst);
}
CDM_BIND::ScalarMassPerAmountData* PBProperty::Unload(const SEScalarMassPerAmount& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarMassPerAmountData* dst = new CDM_BIND::ScalarMassPerAmountData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarMassPerAmount& src, CDM_BIND::ScalarMassPerAmountData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarmassperamount());
}

void PBProperty::Load(const CDM_BIND::ScalarMassPerAreaTimeData& src, SEScalarMassPerAreaTime& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarMassPerAreaTimeData& src, SEScalarMassPerAreaTime& dst)
{
  PBProperty::Serialize(src.scalarmassperareatime(), dst);
}
CDM_BIND::ScalarMassPerAreaTimeData* PBProperty::Unload(const SEScalarMassPerAreaTime& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarMassPerAreaTimeData* dst = new CDM_BIND::ScalarMassPerAreaTimeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarMassPerAreaTime& src, CDM_BIND::ScalarMassPerAreaTimeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarmassperareatime());
}

void PBProperty::Load(const CDM_BIND::ScalarMassPerMassData& src, SEScalarMassPerMass& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarMassPerMassData& src, SEScalarMassPerMass& dst)
{
  PBProperty::Serialize(src.scalarmasspermass(), dst);
}
CDM_BIND::ScalarMassPerMassData* PBProperty::Unload(const SEScalarMassPerMass& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarMassPerMassData* dst = new CDM_BIND::ScalarMassPerMassData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarMassPerMass& src, CDM_BIND::ScalarMassPerMassData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarmasspermass());
}

void PBProperty::Load(const CDM_BIND::ScalarMassPerTimeData& src, SEScalarMassPerTime& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarMassPerTimeData& src, SEScalarMassPerTime& dst)
{
  PBProperty::Serialize(src.scalarmasspertime(), dst);
}
CDM_BIND::ScalarMassPerTimeData* PBProperty::Unload(const SEScalarMassPerTime& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarMassPerTimeData* dst = new CDM_BIND::ScalarMassPerTimeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarMassPerTime& src, CDM_BIND::ScalarMassPerTimeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarmasspertime());
}

void PBProperty::Load(const CDM_BIND::ScalarMassPerVolumeData& src, SEScalarMassPerVolume& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarMassPerVolumeData& src, SEScalarMassPerVolume& dst)
{
  PBProperty::Serialize(src.scalarmasspervolume(), dst);
}
CDM_BIND::ScalarMassPerVolumeData* PBProperty::Unload(const SEScalarMassPerVolume& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarMassPerVolumeData* dst = new CDM_BIND::ScalarMassPerVolumeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarMassPerVolume& src, CDM_BIND::ScalarMassPerVolumeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarmasspervolume());
}

void PBProperty::Load(const CDM_BIND::ScalarNegative1To1Data& src, SEScalarNegative1To1& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarNegative1To1Data& src, SEScalarNegative1To1& dst)
{
  PBProperty::Serialize(src.scalarnegative1to1(), dst);
}
CDM_BIND::ScalarNegative1To1Data* PBProperty::Unload(const SEScalarNegative1To1& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarNegative1To1Data* dst = new CDM_BIND::ScalarNegative1To1Data();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarNegative1To1& src, CDM_BIND::ScalarNegative1To1Data& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarnegative1to1());
}

void PBProperty::Load(const CDM_BIND::ScalarOsmolalityData& src, SEScalarOsmolality& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarOsmolalityData& src, SEScalarOsmolality& dst)
{
  PBProperty::Serialize(src.scalarosmolality(), dst);
}
CDM_BIND::ScalarOsmolalityData* PBProperty::Unload(const SEScalarOsmolality& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarOsmolalityData* dst = new CDM_BIND::ScalarOsmolalityData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarOsmolality& src, CDM_BIND::ScalarOsmolalityData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarosmolality());
}

void PBProperty::Load(const CDM_BIND::ScalarOsmolarityData& src, SEScalarOsmolarity& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarOsmolarityData& src, SEScalarOsmolarity& dst)
{
  PBProperty::Serialize(src.scalarosmolarity(), dst);
}
CDM_BIND::ScalarOsmolarityData* PBProperty::Unload(const SEScalarOsmolarity& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarOsmolarityData* dst = new CDM_BIND::ScalarOsmolarityData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarOsmolarity& src, CDM_BIND::ScalarOsmolarityData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarosmolarity());
}

void PBProperty::Load(const CDM_BIND::ScalarPowerData& src, SEScalarPower& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarPowerData& src, SEScalarPower& dst)
{
  PBProperty::Serialize(src.scalarpower(), dst);
}
CDM_BIND::ScalarPowerData* PBProperty::Unload(const SEScalarPower& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarPowerData* dst = new CDM_BIND::ScalarPowerData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarPower& src, CDM_BIND::ScalarPowerData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarpower());
}

void PBProperty::Load(const CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData& src, SEScalarPowerPerAreaTemperatureToTheFourth& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData& src, SEScalarPowerPerAreaTemperatureToTheFourth& dst)
{
  PBProperty::Serialize(src.scalarpowerperareatemperaturetothefourth(), dst);
}
CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData* PBProperty::Unload(const SEScalarPowerPerAreaTemperatureToTheFourth& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData* dst = new CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarPowerPerAreaTemperatureToTheFourth& src, CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarpowerperareatemperaturetothefourth());
}

void PBProperty::Load(const CDM_BIND::ScalarPressureData& src, SEScalarPressure& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarPressureData& src, SEScalarPressure& dst)
{
  PBProperty::Serialize(src.scalarpressure(), dst);
}
CDM_BIND::ScalarPressureData* PBProperty::Unload(const SEScalarPressure& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarPressureData* dst = new CDM_BIND::ScalarPressureData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarPressure& src, CDM_BIND::ScalarPressureData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarpressure());
}

void PBProperty::Load(const CDM_BIND::ScalarPressurePerVolumeData& src, SEScalarPressurePerVolume& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarPressurePerVolumeData& src, SEScalarPressurePerVolume& dst)
{
  PBProperty::Serialize(src.scalarpressurepervolume(), dst);
}
CDM_BIND::ScalarPressurePerVolumeData* PBProperty::Unload(const SEScalarPressurePerVolume& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarPressurePerVolumeData* dst = new CDM_BIND::ScalarPressurePerVolumeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarPressurePerVolume& src, CDM_BIND::ScalarPressurePerVolumeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarpressurepervolume());
}

void PBProperty::Load(const CDM_BIND::ScalarPressureTimePerAreaData& src, SEScalarPressureTimePerArea& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarPressureTimePerAreaData& src, SEScalarPressureTimePerArea& dst)
{
  PBProperty::Serialize(src.scalarpressuretimeperarea(), dst);
}
CDM_BIND::ScalarPressureTimePerAreaData* PBProperty::Unload(const SEScalarPressureTimePerArea& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarPressureTimePerAreaData* dst = new CDM_BIND::ScalarPressureTimePerAreaData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarPressureTimePerArea& src, CDM_BIND::ScalarPressureTimePerAreaData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarpressuretimeperarea());
}

void PBProperty::Load(const CDM_BIND::ScalarPressureTimePerVolumeData& src, SEScalarPressureTimePerVolume& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarPressureTimePerVolumeData& src, SEScalarPressureTimePerVolume& dst)
{
  PBProperty::Serialize(src.scalarpressuretimepervolume(), dst);
}
CDM_BIND::ScalarPressureTimePerVolumeData* PBProperty::Unload(const SEScalarPressureTimePerVolume& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarPressureTimePerVolumeData* dst = new CDM_BIND::ScalarPressureTimePerVolumeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarPressureTimePerVolume& src, CDM_BIND::ScalarPressureTimePerVolumeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarpressuretimepervolume());
}

void PBProperty::Load(const CDM_BIND::ScalarPressureTimePerVolumeAreaData& src, SEScalarPressureTimePerVolumeArea& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarPressureTimePerVolumeAreaData& src, SEScalarPressureTimePerVolumeArea& dst)
{
  PBProperty::Serialize(src.scalarpressuretimepervolumearea(), dst);
}
CDM_BIND::ScalarPressureTimePerVolumeAreaData* PBProperty::Unload(const SEScalarPressureTimePerVolumeArea& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarPressureTimePerVolumeAreaData* dst = new CDM_BIND::ScalarPressureTimePerVolumeAreaData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarPressureTimePerVolumeArea& src, CDM_BIND::ScalarPressureTimePerVolumeAreaData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarpressuretimepervolumearea());
}

void PBProperty::Load(const CDM_BIND::ScalarPressureTimeSquaredPerVolumeData& src, SEScalarPressureTimeSquaredPerVolume& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarPressureTimeSquaredPerVolumeData& src, SEScalarPressureTimeSquaredPerVolume& dst)
{
  PBProperty::Serialize(src.scalarpressuretimesquaredpervolume(), dst);
}
CDM_BIND::ScalarPressureTimeSquaredPerVolumeData* PBProperty::Unload(const SEScalarPressureTimeSquaredPerVolume& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarPressureTimeSquaredPerVolumeData* dst = new CDM_BIND::ScalarPressureTimeSquaredPerVolumeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarPressureTimeSquaredPerVolume& src, CDM_BIND::ScalarPressureTimeSquaredPerVolumeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarpressuretimesquaredpervolume());
}

void PBProperty::Load(const CDM_BIND::ScalarTemperatureData& src, SEScalarTemperature& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarTemperatureData& src, SEScalarTemperature& dst)
{
  PBProperty::Serialize(src.scalartemperature(), dst);
}
CDM_BIND::ScalarTemperatureData* PBProperty::Unload(const SEScalarTemperature& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarTemperatureData* dst = new CDM_BIND::ScalarTemperatureData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarTemperature& src, CDM_BIND::ScalarTemperatureData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalartemperature());
}

void PBProperty::Load(const CDM_BIND::ScalarTimeData& src, SEScalarTime& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarTimeData& src, SEScalarTime& dst)
{
  PBProperty::Serialize(src.scalartime(), dst);
}
CDM_BIND::ScalarTimeData* PBProperty::Unload(const SEScalarTime& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarTimeData* dst = new CDM_BIND::ScalarTimeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarTime& src, CDM_BIND::ScalarTimeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalartime());
}

void PBProperty::Load(const CDM_BIND::ScalarUnsignedData& src, SEScalarUnsigned& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarUnsignedData& src, SEScalarUnsigned& dst)
{
  PBProperty::Serialize(src.scalarunsigned(), dst);
}
CDM_BIND::ScalarUnsignedData* PBProperty::Unload(const SEScalarUnsigned& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarUnsignedData* dst = new CDM_BIND::ScalarUnsignedData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarUnsigned& src, CDM_BIND::ScalarUnsignedData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarunsigned());
}

void PBProperty::Load(const CDM_BIND::ScalarVolumeData& src, SEScalarVolume& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarVolumeData& src, SEScalarVolume& dst)
{
  PBProperty::Serialize(src.scalarvolume(), dst);
}
CDM_BIND::ScalarVolumeData* PBProperty::Unload(const SEScalarVolume& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarVolumeData* dst = new CDM_BIND::ScalarVolumeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarVolume& src, CDM_BIND::ScalarVolumeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarvolume());
}

void PBProperty::Load(const CDM_BIND::ScalarVolumePerPressureData& src, SEScalarVolumePerPressure& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarVolumePerPressureData& src, SEScalarVolumePerPressure& dst)
{
  PBProperty::Serialize(src.scalarvolumeperpressure(), dst);
}
CDM_BIND::ScalarVolumePerPressureData* PBProperty::Unload(const SEScalarVolumePerPressure& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarVolumePerPressureData* dst = new CDM_BIND::ScalarVolumePerPressureData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarVolumePerPressure& src, CDM_BIND::ScalarVolumePerPressureData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarvolumeperpressure());
}

void PBProperty::Load(const CDM_BIND::ScalarVolumePerTimeData& src, SEScalarVolumePerTime& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarVolumePerTimeData& src, SEScalarVolumePerTime& dst)
{
  PBProperty::Serialize(src.scalarvolumepertime(), dst);
}
CDM_BIND::ScalarVolumePerTimeData* PBProperty::Unload(const SEScalarVolumePerTime& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarVolumePerTimeData* dst = new CDM_BIND::ScalarVolumePerTimeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarVolumePerTime& src, CDM_BIND::ScalarVolumePerTimeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarvolumepertime());
}

void PBProperty::Load(const CDM_BIND::ScalarVolumePerTimeAreaData& src, SEScalarVolumePerTimeArea& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarVolumePerTimeAreaData& src, SEScalarVolumePerTimeArea& dst)
{
  PBProperty::Serialize(src.scalarvolumepertimearea(), dst);
}
CDM_BIND::ScalarVolumePerTimeAreaData* PBProperty::Unload(const SEScalarVolumePerTimeArea& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarVolumePerTimeAreaData* dst = new CDM_BIND::ScalarVolumePerTimeAreaData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarVolumePerTimeArea& src, CDM_BIND::ScalarVolumePerTimeAreaData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarvolumepertimearea());
}

void PBProperty::Load(const CDM_BIND::ScalarVolumePerTimeMassData& src, SEScalarVolumePerTimeMass& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarVolumePerTimeMassData& src, SEScalarVolumePerTimeMass& dst)
{
  PBProperty::Serialize(src.scalarvolumepertimemass(), dst);
}
CDM_BIND::ScalarVolumePerTimeMassData* PBProperty::Unload(const SEScalarVolumePerTimeMass& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarVolumePerTimeMassData* dst = new CDM_BIND::ScalarVolumePerTimeMassData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarVolumePerTimeMass& src, CDM_BIND::ScalarVolumePerTimeMassData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarvolumepertimemass());
}

void PBProperty::Load(const CDM_BIND::ScalarVolumePerTimePressureData& src, SEScalarVolumePerTimePressure& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarVolumePerTimePressureData& src, SEScalarVolumePerTimePressure& dst)
{
  PBProperty::Serialize(src.scalarvolumepertimepressure(), dst);
}
CDM_BIND::ScalarVolumePerTimePressureData* PBProperty::Unload(const SEScalarVolumePerTimePressure& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarVolumePerTimePressureData* dst = new CDM_BIND::ScalarVolumePerTimePressureData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarVolumePerTimePressure& src, CDM_BIND::ScalarVolumePerTimePressureData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarvolumepertimepressure());
}

void PBProperty::Load(const CDM_BIND::ScalarVolumePerTimePressureAreaData& src, SEScalarVolumePerTimePressureArea& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ScalarVolumePerTimePressureAreaData& src, SEScalarVolumePerTimePressureArea& dst)
{
  PBProperty::Serialize(src.scalarvolumepertimepressurearea(), dst);
}

CDM_BIND::ScalarVolumePerTimePressureAreaData* PBProperty::Unload(const SEScalarVolumePerTimePressureArea& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ScalarVolumePerTimePressureAreaData* dst = new CDM_BIND::ScalarVolumePerTimePressureAreaData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEScalarVolumePerTimePressureArea& src, CDM_BIND::ScalarVolumePerTimePressureAreaData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_scalarvolumepertimepressurearea());
}


////////////
// Arrays //
////////////

void PBProperty::Load(const CDM_BIND::ArrayData& src, SEArray& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);

  if (!src.unit().empty())
  {
    if (src.unit() != "unitless")
      throw CommonDataModelException("CDM_BIND::Array API is intended to be unitless, You are trying to load an array with a unit defined");
  }
}
void PBProperty::Serialize(const CDM_BIND::ArrayData& src, SEArray& dst)
{
  for (int i = 0; i < src.value().value_size(); i++)
    dst.m_Data.push_back(src.value().value(i));
}
CDM_BIND::ArrayData* PBProperty::Unload(const SEArray& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ArrayData* dst = new CDM_BIND::ArrayData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEArray& src, CDM_BIND::ArrayData& dst)
{
  for (size_t i = 0; i < src.m_Data.size(); i++)
  {
    dst.mutable_value()->add_value(src.m_Data[i]);
  }
}

void PBProperty::Load(const CDM_BIND::ArrayElectricPotentialData& src, SEArrayElectricPotential& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::ArrayElectricPotentialData& src, SEArrayElectricPotential& dst)
{
  PBProperty::Serialize(src.arrayelectricpotential(), dst);
  dst.m_ElectricPotentialUnit = &ElectricPotentialUnit::GetCompoundUnit(src.arrayelectricpotential().unit());
}

CDM_BIND::ArrayElectricPotentialData* PBProperty::Unload(const SEArrayElectricPotential& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::ArrayElectricPotentialData* dst = new CDM_BIND::ArrayElectricPotentialData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEArrayElectricPotential& src, CDM_BIND::ArrayElectricPotentialData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_arrayelectricpotential());
  dst.mutable_arrayelectricpotential()->set_unit(src.m_ElectricPotentialUnit->GetString());
}


///////////////
// Functions //
///////////////

void PBProperty::Load(const CDM_BIND::FunctionData& src, SEFunction& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);

  if (!src.dependentunit().empty())
  {
    if (src.dependentunit() != "unitless")
      throw CommonDataModelException("CDM_BIND::Function API is intended to be unitless, You are trying to load a dependent axis with a unit defined");
  }
  if (!src.independentunit().empty())
  {
    if (src.independentunit() != "unitless")
      throw CommonDataModelException("CDM_BIND::Function API is intended to be unitless, You are trying to load an independent axis with a unit defined");
  }
}
void PBProperty::Serialize(const CDM_BIND::FunctionData& src, SEFunction& dst)
{
  for (int i = 0; i < src.dependent().value_size(); i++)
    dst.m_Dependent.push_back(src.dependent().value(i));
  for (int i = 0; i < src.independent().value_size(); i++)
    dst.m_Independent.push_back(src.independent().value(i));
}
CDM_BIND::FunctionData* PBProperty::Unload(const SEFunction& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::FunctionData* dst = new CDM_BIND::FunctionData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEFunction& src, CDM_BIND::FunctionData& dst)
{
  for (size_t i = 0; i < src.m_Dependent.size(); i++)
  {
    dst.mutable_dependent()->add_value(src.m_Dependent[i]);
    dst.mutable_independent()->add_value(src.m_Independent[i]);
  }
}

void PBProperty::Load(const CDM_BIND::FunctionElectricPotentialVsTimeData& src, SEFunctionElectricPotentialVsTime& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::FunctionElectricPotentialVsTimeData& src, SEFunctionElectricPotentialVsTime& dst)
{
  PBProperty::Serialize(src.functionelectricpotentialvstime(), dst);
  dst.m_TimeUnit = &TimeUnit::GetCompoundUnit(src.functionelectricpotentialvstime().independentunit());
  dst.m_ElectricPotentialUnit = &ElectricPotentialUnit::GetCompoundUnit(src.functionelectricpotentialvstime().dependentunit());
}

CDM_BIND::FunctionElectricPotentialVsTimeData* PBProperty::Unload(const SEFunctionElectricPotentialVsTime& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::FunctionElectricPotentialVsTimeData* dst = new CDM_BIND::FunctionElectricPotentialVsTimeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEFunctionElectricPotentialVsTime& src, CDM_BIND::FunctionElectricPotentialVsTimeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_functionelectricpotentialvstime());
  dst.mutable_functionelectricpotentialvstime()->set_independentunit(src.m_TimeUnit->GetString());
  dst.mutable_functionelectricpotentialvstime()->set_dependentunit(src.m_ElectricPotentialUnit->GetString());
}

void PBProperty::Load(const CDM_BIND::FunctionVolumeVsTimeData& src, SEFunctionVolumeVsTime& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::FunctionVolumeVsTimeData& src, SEFunctionVolumeVsTime& dst)
{
  PBProperty::Serialize(src.functionvolumevstime(), dst);
  dst.m_TimeUnit = &TimeUnit::GetCompoundUnit(src.functionvolumevstime().independentunit());
  dst.m_VolumeUnit = &VolumeUnit::GetCompoundUnit(src.functionvolumevstime().dependentunit());
}
CDM_BIND::FunctionVolumeVsTimeData* PBProperty::Unload(const SEFunctionVolumeVsTime& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::FunctionVolumeVsTimeData* dst = new CDM_BIND::FunctionVolumeVsTimeData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEFunctionVolumeVsTime& src, CDM_BIND::FunctionVolumeVsTimeData& dst)
{
  PBProperty::Serialize(src, *dst.mutable_functionvolumevstime());
  dst.mutable_functionvolumevstime()->set_independentunit(src.m_TimeUnit->GetString());
  dst.mutable_functionvolumevstime()->set_dependentunit(src.m_VolumeUnit->GetString());
}

////////////////
// Histograms //
////////////////


void PBProperty::Load(const CDM_BIND::HistogramData& src, SEHistogram& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);

  if (!src.histogram().dependentunit().empty())
  {
    if (src.histogram().dependentunit() != "unitless")
      throw CommonDataModelException("CDM_BIND::Histogram API is intended to be unitless, You are trying to load a dependent axis with a unit defined");
  }
  if (!src.histogram().independentunit().empty())
  {
    if (src.histogram().independentunit() != "unitless")
      throw CommonDataModelException("CDM_BIND::Histogram API is intended to be unitless, You are trying to load an independent axis with a unit defined");
  }
}
void PBProperty::Serialize(const CDM_BIND::HistogramData& src, SEHistogram& dst)
{
  for (int i = 0; i < src.histogram().dependent().value_size(); i++)
    dst.m_Dependent.push_back(src.histogram().dependent().value(i));
  for (int i = 0; i < src.histogram().independent().value_size(); i++)
    dst.m_Independent.push_back(src.histogram().independent().value(i));
}

CDM_BIND::HistogramData* PBProperty::Unload(const SEHistogram& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::HistogramData* dst = new CDM_BIND::HistogramData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEHistogram& src, CDM_BIND::HistogramData& dst)
{
  for (size_t i = 0; i < src.m_Dependent.size(); i++)
  {
    dst.mutable_histogram()->mutable_dependent()->add_value(src.m_Dependent[i]);
    dst.mutable_histogram()->mutable_independent()->add_value(src.m_Independent[i]);
  }
}

void PBProperty::Load(const CDM_BIND::HistogramFractionVsLengthData& src, SEHistogramFractionVsLength& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::HistogramFractionVsLengthData& src, SEHistogramFractionVsLength& dst)
{
  for (int i = 0; i < src.histogramfractionvslength().histogram().dependent().value_size(); i++)
    dst.m_Dependent.push_back(src.histogramfractionvslength().histogram().dependent().value(i));
  for (int i = 0; i < src.histogramfractionvslength().histogram().independent().value_size(); i++)
    dst.m_Independent.push_back(src.histogramfractionvslength().histogram().independent().value(i));
}
CDM_BIND::HistogramFractionVsLengthData* PBProperty::Unload(const SEHistogramFractionVsLength& src)
{
  if (!src.IsValid())
    return nullptr;
  CDM_BIND::HistogramFractionVsLengthData* dst = new CDM_BIND::HistogramFractionVsLengthData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SEHistogramFractionVsLength& src, CDM_BIND::HistogramFractionVsLengthData& dst)
{
  for (size_t i = 0; i < src.m_Dependent.size(); i++)
    dst.mutable_histogramfractionvslength()->mutable_histogram()->mutable_dependent()->add_value(src.m_Dependent[i]);
  for (size_t i = 0; i < src.m_Independent.size(); i++)
    dst.mutable_histogramfractionvslength()->mutable_histogram()->mutable_independent()->add_value(src.m_Independent[i]);
}

void PBProperty::Load(const CDM_BIND::RunningAverageData& src, SERunningAverage& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::RunningAverageData& src, SERunningAverage& dst)
{
  dst.m_Sum = src.sum();
  dst.m_NumSamples = src.numsamples();
}
CDM_BIND::RunningAverageData* PBProperty::Unload(const SERunningAverage& src)
{
  CDM_BIND::RunningAverageData* dst = new CDM_BIND::RunningAverageData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SERunningAverage& src, CDM_BIND::RunningAverageData& dst)
{
  dst.set_sum(src.m_Sum);
  dst.set_numsamples(src.m_NumSamples);
}

void PBProperty::Load(const CDM_BIND::TemporalInterpolatorData& src, SETemporalInterpolator& dst)
{
  dst.Invalidate();
  PBProperty::Serialize(src, dst);
}
void PBProperty::Serialize(const CDM_BIND::TemporalInterpolatorData& src, SETemporalInterpolator& dst)
{
  dst.m_Complete = src.complete();
  dst.m_PositiveTransition = src.positivetransition();
  dst.m_Period_s = src.period_s();
  dst.m_Current = src.current();
  dst.m_Target = src.target();
  dst.m_Initial = src.initial();
}
CDM_BIND::TemporalInterpolatorData* PBProperty::Unload(const SETemporalInterpolator& src)
{
  CDM_BIND::TemporalInterpolatorData* dst = new CDM_BIND::TemporalInterpolatorData();
  PBProperty::Serialize(src, *dst);
  return dst;
}
void PBProperty::Serialize(const SETemporalInterpolator& src, CDM_BIND::TemporalInterpolatorData& dst)
{
  dst.set_complete(src.m_Complete);
  dst.set_positivetransition(src.m_PositiveTransition);
  dst.set_period_s(src.m_Period_s);
  dst.set_current(src.m_Current);
  dst.set_target(src.m_Target);
  dst.set_initial(src.m_Initial);
}