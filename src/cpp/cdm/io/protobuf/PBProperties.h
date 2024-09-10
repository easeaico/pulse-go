/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
   // Forward Declaire SE and Protobuf Property Types
CDM_BIND_DECL(CurveData)
CDM_BIND_DECL(SegmentConstantData)
CDM_BIND_DECL(SegmentLinearData)
CDM_BIND_DECL(SegmentParabolicData)
CDM_BIND_DECL(SegmentSigmoidalData)

CDM_BIND_DECL(ScalarData)
CDM_BIND_DECL(Scalar0To1Data)
CDM_BIND_DECL(ScalarAmountData)
CDM_BIND_DECL(ScalarAmountPerMassData)
CDM_BIND_DECL(ScalarAmountPerTimeData)
CDM_BIND_DECL(ScalarAmountPerVolumeData)
CDM_BIND_DECL(ScalarAreaData)
CDM_BIND_DECL(ScalarAreaPerTimePressureData)
CDM_BIND_DECL(ScalarElectricCapacitanceData)
CDM_BIND_DECL(ScalarElectricChargeData)
CDM_BIND_DECL(ScalarElectricCurrentData)
CDM_BIND_DECL(ScalarElectricInductanceData)
CDM_BIND_DECL(ScalarElectricPotentialData)
CDM_BIND_DECL(ScalarElectricResistanceData)
CDM_BIND_DECL(ScalarEnergyData)
CDM_BIND_DECL(ScalarEnergyPerAmountData)
CDM_BIND_DECL(ScalarEnergyPerMassData)
CDM_BIND_DECL(ScalarEquivalentWeightPerVolumeData)
CDM_BIND_DECL(ScalarForceData)
CDM_BIND_DECL(ScalarFrequencyData)
CDM_BIND_DECL(ScalarHeatCapacitanceData)
CDM_BIND_DECL(ScalarHeatCapacitancePerAmountData)
CDM_BIND_DECL(ScalarHeatCapacitancePerMassData)
CDM_BIND_DECL(ScalarHeatConductanceData)
CDM_BIND_DECL(ScalarHeatConductancePerAreaData)
CDM_BIND_DECL(ScalarHeatInductanceData)
CDM_BIND_DECL(ScalarHeatResistanceData)
CDM_BIND_DECL(ScalarHeatResistanceAreaData)
CDM_BIND_DECL(ScalarInversePressureData)
CDM_BIND_DECL(ScalarInverseVolumeData)
CDM_BIND_DECL(ScalarLengthData)
CDM_BIND_DECL(ScalarLengthPerTimeData)
CDM_BIND_DECL(ScalarLengthPerTimePressureData)
CDM_BIND_DECL(ScalarMassData)
CDM_BIND_DECL(ScalarMassPerAmountData)
CDM_BIND_DECL(ScalarMassPerAreaTimeData)
CDM_BIND_DECL(ScalarMassPerMassData)
CDM_BIND_DECL(ScalarMassPerTimeData)
CDM_BIND_DECL(ScalarMassPerVolumeData)
CDM_BIND_DECL(ScalarNegative1To1Data)
CDM_BIND_DECL(ScalarOsmolalityData)
CDM_BIND_DECL(ScalarOsmolarityData)
CDM_BIND_DECL(ScalarPowerData)
CDM_BIND_DECL(ScalarPowerPerAreaTemperatureToTheFourthData)
CDM_BIND_DECL(ScalarPressureData)
CDM_BIND_DECL(ScalarPressurePerVolumeData)
CDM_BIND_DECL(ScalarPressureTimePerVolumeData)
CDM_BIND_DECL(ScalarPressureTimePerAreaData)
CDM_BIND_DECL(ScalarPressureTimePerVolumeAreaData)
CDM_BIND_DECL(ScalarPressureTimeSquaredPerVolumeData)
CDM_BIND_DECL(ScalarTemperatureData)
CDM_BIND_DECL(ScalarTimeData)
CDM_BIND_DECL(ScalarUnsignedData)
CDM_BIND_DECL(ScalarVolumeData)
CDM_BIND_DECL(ScalarVolumePerPressureData)
CDM_BIND_DECL(ScalarVolumePerTimeData)
CDM_BIND_DECL(ScalarVolumePerTimeAreaData)
CDM_BIND_DECL(ScalarVolumePerTimeMassData)
CDM_BIND_DECL(ScalarVolumePerTimePressureData)
CDM_BIND_DECL(ScalarVolumePerTimePressureAreaData)

CDM_BIND_DECL(ArrayData)
CDM_BIND_DECL(ArrayElectricPotentialData)

CDM_BIND_DECL(FunctionData)
CDM_BIND_DECL(FunctionElectricPotentialVsTimeData)
CDM_BIND_DECL(FunctionVolumeVsTimeData)

CDM_BIND_DECL(HistogramData)
CDM_BIND_DECL(HistogramFractionVsLengthData)

CDM_BIND_DECL2(RunningAverage)
CDM_BIND_DECL2(TemporalInterpolator)

class CDM_DECL PBProperty
{
public:
  // Curve/Segments
  static void Copy(const SECurve& src, SECurve& dst);
  static void Load(const CDM_BIND::CurveData& src, SECurve& dst);
  static CDM_BIND::CurveData* Unload(const SECurve& src);
  static void Serialize(const CDM_BIND::CurveData& src, SECurve& dst);
  static void Serialize(const SECurve& src, CDM_BIND::CurveData& dst);

  static void Load(const CDM_BIND::SegmentConstantData& src, SESegmentConstant& dst);
  static CDM_BIND::SegmentConstantData* Unload(const SESegmentConstant& src);
  static void Serialize(const CDM_BIND::SegmentConstantData& src, SESegmentConstant& dst);
  static void Serialize(const SESegmentConstant& src, CDM_BIND::SegmentConstantData& dst);

  static void Load(const CDM_BIND::SegmentLinearData& src, SESegmentLinear& dst);
  static CDM_BIND::SegmentLinearData* Unload(const SESegmentLinear& src);
  static void Serialize(const CDM_BIND::SegmentLinearData& src, SESegmentLinear& dst);
  static void Serialize(const SESegmentLinear& src, CDM_BIND::SegmentLinearData& dst);

  static void Load(const CDM_BIND::SegmentParabolicData& src, SESegmentParabolic& dst);
  static CDM_BIND::SegmentParabolicData* Unload(const SESegmentParabolic& src);
  static void Serialize(const CDM_BIND::SegmentParabolicData& src, SESegmentParabolic& dst);
  static void Serialize(const SESegmentParabolic& src, CDM_BIND::SegmentParabolicData& dst);

  static void Load(const CDM_BIND::SegmentSigmoidalData& src, SESegmentSigmoidal& dst);
  static CDM_BIND::SegmentSigmoidalData* Unload(const SESegmentSigmoidal& src);
  static void Serialize(const CDM_BIND::SegmentSigmoidalData& src, SESegmentSigmoidal& dst);
  static void Serialize(const SESegmentSigmoidal& src, CDM_BIND::SegmentSigmoidalData& dst);

  // Scalars
  static void Load(const CDM_BIND::ScalarData& src, SEScalar& dst);
  static CDM_BIND::ScalarData* Unload(const SEScalar& src);
  static void Serialize(const CDM_BIND::ScalarData& src, SEScalar& dst);
  static void Serialize(const SEScalar& src, CDM_BIND::ScalarData& dst);
  static void Serialize(const CDM_BIND::ScalarData& src, SEUnitScalar& dst);
  static void Serialize(const SEUnitScalar& src, CDM_BIND::ScalarData& dst);

  static void Load(const CDM_BIND::Scalar0To1Data& src, SEScalar0To1& dst);
  static CDM_BIND::Scalar0To1Data* Unload(const SEScalar0To1& src);
  static void Serialize(const CDM_BIND::Scalar0To1Data& src, SEScalar0To1& dst);
  static void Serialize(const SEScalar0To1& src, CDM_BIND::Scalar0To1Data& dst);

  static void Load(const CDM_BIND::ScalarAmountData& src, SEScalarAmount& dst);
  static CDM_BIND::ScalarAmountData* Unload(const SEScalarAmount& src);
  static void Serialize(const CDM_BIND::ScalarAmountData& src, SEScalarAmount& dst);
  static void Serialize(const SEScalarAmount& src, CDM_BIND::ScalarAmountData& dst);

  static void Load(const CDM_BIND::ScalarAmountPerMassData& src, SEScalarAmountPerMass& dst);
  static CDM_BIND::ScalarAmountPerMassData* Unload(const SEScalarAmountPerMass& src);
  static void Serialize(const CDM_BIND::ScalarAmountPerMassData& src, SEScalarAmountPerMass& dst);
  static void Serialize(const SEScalarAmountPerMass& src, CDM_BIND::ScalarAmountPerMassData& dst);

  static void Load(const CDM_BIND::ScalarAmountPerTimeData& src, SEScalarAmountPerTime& dst);
  static CDM_BIND::ScalarAmountPerTimeData* Unload(const SEScalarAmountPerTime& src);
  static void Serialize(const CDM_BIND::ScalarAmountPerTimeData& src, SEScalarAmountPerTime& dst);
  static void Serialize(const SEScalarAmountPerTime& src, CDM_BIND::ScalarAmountPerTimeData& dst);

  static void Load(const CDM_BIND::ScalarAmountPerVolumeData& src, SEScalarAmountPerVolume& dst);
  static CDM_BIND::ScalarAmountPerVolumeData* Unload(const SEScalarAmountPerVolume& src);
  static void Serialize(const CDM_BIND::ScalarAmountPerVolumeData& src, SEScalarAmountPerVolume& dst);
  static void Serialize(const SEScalarAmountPerVolume& src, CDM_BIND::ScalarAmountPerVolumeData& dst);

  static void Load(const CDM_BIND::ScalarAreaData& src, SEScalarArea& dst);
  static CDM_BIND::ScalarAreaData* Unload(const SEScalarArea& src);
  static void Serialize(const CDM_BIND::ScalarAreaData& src, SEScalarArea& dst);
  static void Serialize(const SEScalarArea& src, CDM_BIND::ScalarAreaData& dst);

  static void Load(const CDM_BIND::ScalarAreaPerTimePressureData& src, SEScalarAreaPerTimePressure& dst);
  static CDM_BIND::ScalarAreaPerTimePressureData* Unload(const SEScalarAreaPerTimePressure& src);
  static void Serialize(const CDM_BIND::ScalarAreaPerTimePressureData& src, SEScalarAreaPerTimePressure& dst);
  static void Serialize(const SEScalarAreaPerTimePressure& src, CDM_BIND::ScalarAreaPerTimePressureData& dst);

  static void Load(const CDM_BIND::ScalarElectricCapacitanceData& src, SEScalarElectricCapacitance& dst);
  static CDM_BIND::ScalarElectricCapacitanceData* Unload(const SEScalarElectricCapacitance& src);
  static void Serialize(const CDM_BIND::ScalarElectricCapacitanceData& src, SEScalarElectricCapacitance& dst);
  static void Serialize(const SEScalarElectricCapacitance& src, CDM_BIND::ScalarElectricCapacitanceData& dst);

  static void Load(const CDM_BIND::ScalarElectricChargeData& src, SEScalarElectricCharge& dst);
  static CDM_BIND::ScalarElectricChargeData* Unload(const SEScalarElectricCharge& src);
  static void Serialize(const CDM_BIND::ScalarElectricChargeData& src, SEScalarElectricCharge& dst);
  static void Serialize(const SEScalarElectricCharge& src, CDM_BIND::ScalarElectricChargeData& dst);

  static void Load(const CDM_BIND::ScalarElectricCurrentData& src, SEScalarElectricCurrent& dst);
  static CDM_BIND::ScalarElectricCurrentData* Unload(const SEScalarElectricCurrent& src);
  static void Serialize(const CDM_BIND::ScalarElectricCurrentData& src, SEScalarElectricCurrent& dst);
  static void Serialize(const SEScalarElectricCurrent& src, CDM_BIND::ScalarElectricCurrentData& dst);

  static void Load(const CDM_BIND::ScalarElectricInductanceData& src, SEScalarElectricInductance& dst);
  static CDM_BIND::ScalarElectricInductanceData* Unload(const SEScalarElectricInductance& src);
  static void Serialize(const CDM_BIND::ScalarElectricInductanceData& src, SEScalarElectricInductance& dst);
  static void Serialize(const SEScalarElectricInductance& src, CDM_BIND::ScalarElectricInductanceData& dst);

  static void Load(const CDM_BIND::ScalarElectricPotentialData& src, SEScalarElectricPotential& dst);
  static CDM_BIND::ScalarElectricPotentialData* Unload(const SEScalarElectricPotential& src);
  static void Serialize(const CDM_BIND::ScalarElectricPotentialData& src, SEScalarElectricPotential& dst);
  static void Serialize(const SEScalarElectricPotential& src, CDM_BIND::ScalarElectricPotentialData& dst);

  static void Load(const CDM_BIND::ScalarElectricResistanceData& src, SEScalarElectricResistance& dst);
  static CDM_BIND::ScalarElectricResistanceData* Unload(const SEScalarElectricResistance& src);
  static void Serialize(const CDM_BIND::ScalarElectricResistanceData& src, SEScalarElectricResistance& dst);
  static void Serialize(const SEScalarElectricResistance& src, CDM_BIND::ScalarElectricResistanceData& dst);

  static void Load(const CDM_BIND::ScalarEnergyData& src, SEScalarEnergy& dst);
  static CDM_BIND::ScalarEnergyData* Unload(const SEScalarEnergy& src);
  static void Serialize(const CDM_BIND::ScalarEnergyData& src, SEScalarEnergy& dst);
  static void Serialize(const SEScalarEnergy& src, CDM_BIND::ScalarEnergyData& dst);

  static void Load(const CDM_BIND::ScalarEnergyPerAmountData& src, SEScalarEnergyPerAmount& dst);
  static CDM_BIND::ScalarEnergyPerAmountData* Unload(const SEScalarEnergyPerAmount& src);
  static void Serialize(const CDM_BIND::ScalarEnergyPerAmountData& src, SEScalarEnergyPerAmount& dst);
  static void Serialize(const SEScalarEnergyPerAmount& src, CDM_BIND::ScalarEnergyPerAmountData& dst);

  static void Load(const CDM_BIND::ScalarEnergyPerMassData& src, SEScalarEnergyPerMass& dst);
  static CDM_BIND::ScalarEnergyPerMassData* Unload(const SEScalarEnergyPerMass& src);
  static void Serialize(const CDM_BIND::ScalarEnergyPerMassData& src, SEScalarEnergyPerMass& dst);
  static void Serialize(const SEScalarEnergyPerMass& src, CDM_BIND::ScalarEnergyPerMassData& dst);

  static void Load(const CDM_BIND::ScalarEquivalentWeightPerVolumeData& src, SEScalarEquivalentWeightPerVolume& dst);
  static CDM_BIND::ScalarEquivalentWeightPerVolumeData* Unload(const SEScalarEquivalentWeightPerVolume& src);
  static void Serialize(const CDM_BIND::ScalarEquivalentWeightPerVolumeData& src, SEScalarEquivalentWeightPerVolume& dst);
  static void Serialize(const SEScalarEquivalentWeightPerVolume& src, CDM_BIND::ScalarEquivalentWeightPerVolumeData& dst);

  static void Load(const CDM_BIND::ScalarForceData& src, SEScalarForce& dst);
  static CDM_BIND::ScalarForceData* Unload(const SEScalarForce& src);
  static void Serialize(const CDM_BIND::ScalarForceData& src, SEScalarForce& dst);
  static void Serialize(const SEScalarForce& src, CDM_BIND::ScalarForceData& dst);

  static void Load(const CDM_BIND::ScalarFrequencyData& src, SEScalarFrequency& dst);
  static CDM_BIND::ScalarFrequencyData* Unload(const SEScalarFrequency& src);
  static void Serialize(const CDM_BIND::ScalarFrequencyData& src, SEScalarFrequency& dst);
  static void Serialize(const SEScalarFrequency& src, CDM_BIND::ScalarFrequencyData& dst);

  static void Load(const CDM_BIND::ScalarHeatCapacitanceData& src, SEScalarHeatCapacitance& dst);
  static CDM_BIND::ScalarHeatCapacitanceData* Unload(const SEScalarHeatCapacitance& src);
  static void Serialize(const CDM_BIND::ScalarHeatCapacitanceData& src, SEScalarHeatCapacitance& dst);
  static void Serialize(const SEScalarHeatCapacitance& src, CDM_BIND::ScalarHeatCapacitanceData& dst);

  static void Load(const CDM_BIND::ScalarHeatCapacitancePerAmountData& src, SEScalarHeatCapacitancePerAmount& dst);
  static CDM_BIND::ScalarHeatCapacitancePerAmountData* Unload(const SEScalarHeatCapacitancePerAmount& src);
  static void Serialize(const CDM_BIND::ScalarHeatCapacitancePerAmountData& src, SEScalarHeatCapacitancePerAmount& dst);
  static void Serialize(const SEScalarHeatCapacitancePerAmount& src, CDM_BIND::ScalarHeatCapacitancePerAmountData& dst);
  
  static void Load(const CDM_BIND::ScalarHeatCapacitancePerMassData& src, SEScalarHeatCapacitancePerMass& dst);
  static CDM_BIND::ScalarHeatCapacitancePerMassData* Unload(const SEScalarHeatCapacitancePerMass& src);
  static void Serialize(const CDM_BIND::ScalarHeatCapacitancePerMassData& src, SEScalarHeatCapacitancePerMass& dst);
  static void Serialize(const SEScalarHeatCapacitancePerMass& src, CDM_BIND::ScalarHeatCapacitancePerMassData& dst);

  static void Load(const CDM_BIND::ScalarHeatConductanceData& src, SEScalarHeatConductance& dst);
  static CDM_BIND::ScalarHeatConductanceData* Unload(const SEScalarHeatConductance& src);
  static void Serialize(const CDM_BIND::ScalarHeatConductanceData& src, SEScalarHeatConductance& dst);
  static void Serialize(const SEScalarHeatConductance& src, CDM_BIND::ScalarHeatConductanceData& dst);

  static void Load(const CDM_BIND::ScalarHeatConductancePerAreaData& src, SEScalarHeatConductancePerArea& dst);
  static CDM_BIND::ScalarHeatConductancePerAreaData* Unload(const SEScalarHeatConductancePerArea& src);
  static void Serialize(const CDM_BIND::ScalarHeatConductancePerAreaData& src, SEScalarHeatConductancePerArea& dst);
  static void Serialize(const SEScalarHeatConductancePerArea& src, CDM_BIND::ScalarHeatConductancePerAreaData& dst);

  static void Load(const CDM_BIND::ScalarHeatInductanceData& src, SEScalarHeatInductance& dst);
  static CDM_BIND::ScalarHeatInductanceData* Unload(const SEScalarHeatInductance& src);
  static void Serialize(const CDM_BIND::ScalarHeatInductanceData& src, SEScalarHeatInductance& dst);
  static void Serialize(const SEScalarHeatInductance& src, CDM_BIND::ScalarHeatInductanceData& dst);

  static void Load(const CDM_BIND::ScalarHeatResistanceData& src, SEScalarHeatResistance& dst);
  static CDM_BIND::ScalarHeatResistanceData* Unload(const SEScalarHeatResistance& src);
  static void Serialize(const CDM_BIND::ScalarHeatResistanceData& src, SEScalarHeatResistance& dst);
  static void Serialize(const SEScalarHeatResistance& src, CDM_BIND::ScalarHeatResistanceData& dst);

  static void Load(const CDM_BIND::ScalarHeatResistanceAreaData& src, SEScalarHeatResistanceArea& dst);
  static CDM_BIND::ScalarHeatResistanceAreaData* Unload(const SEScalarHeatResistanceArea& src);
  static void Serialize(const CDM_BIND::ScalarHeatResistanceAreaData& src, SEScalarHeatResistanceArea& dst);
  static void Serialize(const SEScalarHeatResistanceArea& src, CDM_BIND::ScalarHeatResistanceAreaData& dst);

  static void Load(const CDM_BIND::ScalarInversePressureData& src, SEScalarInversePressure& dst);
  static CDM_BIND::ScalarInversePressureData* Unload(const SEScalarInversePressure& src);
  static void Serialize(const CDM_BIND::ScalarInversePressureData& src, SEScalarInversePressure& dst);
  static void Serialize(const SEScalarInversePressure& src, CDM_BIND::ScalarInversePressureData& dst);

  static void Load(const CDM_BIND::ScalarInverseVolumeData& src, SEScalarInverseVolume& dst);
  static CDM_BIND::ScalarInverseVolumeData* Unload(const SEScalarInverseVolume& src);
  static void Serialize(const CDM_BIND::ScalarInverseVolumeData& src, SEScalarInverseVolume& dst);
  static void Serialize(const SEScalarInverseVolume& src, CDM_BIND::ScalarInverseVolumeData& dst);

  static void Load(const CDM_BIND::ScalarLengthData& src, SEScalarLength& dst);
  static CDM_BIND::ScalarLengthData* Unload(const SEScalarLength& src);
  static void Serialize(const CDM_BIND::ScalarLengthData& src, SEScalarLength& dst);
  static void Serialize(const SEScalarLength& src, CDM_BIND::ScalarLengthData& dst);

  static void Load(const CDM_BIND::ScalarLengthPerTimeData& src, SEScalarLengthPerTime& dst);
  static CDM_BIND::ScalarLengthPerTimeData* Unload(const SEScalarLengthPerTime& src);
  static void Serialize(const CDM_BIND::ScalarLengthPerTimeData& src, SEScalarLengthPerTime& dst);
  static void Serialize(const SEScalarLengthPerTime& src, CDM_BIND::ScalarLengthPerTimeData& dst);

  static void Load(const CDM_BIND::ScalarLengthPerTimePressureData& src, SEScalarLengthPerTimePressure& dst);
  static CDM_BIND::ScalarLengthPerTimePressureData* Unload(const SEScalarLengthPerTimePressure& src);
  static void Serialize(const CDM_BIND::ScalarLengthPerTimePressureData& src, SEScalarLengthPerTimePressure& dst);
  static void Serialize(const SEScalarLengthPerTimePressure& src, CDM_BIND::ScalarLengthPerTimePressureData& dst);

  static void Load(const CDM_BIND::ScalarMassData& src, SEScalarMass& dst);
  static CDM_BIND::ScalarMassData* Unload(const SEScalarMass& src);
  static void Serialize(const CDM_BIND::ScalarMassData& src, SEScalarMass& dst);
  static void Serialize(const SEScalarMass& src, CDM_BIND::ScalarMassData& dst);

  static void Load(const CDM_BIND::ScalarMassPerAmountData& src, SEScalarMassPerAmount& dst);
  static CDM_BIND::ScalarMassPerAmountData* Unload(const SEScalarMassPerAmount& src);
  static void Serialize(const CDM_BIND::ScalarMassPerAmountData& src, SEScalarMassPerAmount& dst);
  static void Serialize(const SEScalarMassPerAmount& src, CDM_BIND::ScalarMassPerAmountData& dst);

  static void Load(const CDM_BIND::ScalarMassPerAreaTimeData& src, SEScalarMassPerAreaTime& dst);
  static CDM_BIND::ScalarMassPerAreaTimeData* Unload(const SEScalarMassPerAreaTime& src);
  static void Serialize(const CDM_BIND::ScalarMassPerAreaTimeData& src, SEScalarMassPerAreaTime& dst);
  static void Serialize(const SEScalarMassPerAreaTime& src, CDM_BIND::ScalarMassPerAreaTimeData& dst);

  static void Load(const CDM_BIND::ScalarMassPerMassData& src, SEScalarMassPerMass& dst);
  static CDM_BIND::ScalarMassPerMassData* Unload(const SEScalarMassPerMass& src);
  static void Serialize(const CDM_BIND::ScalarMassPerMassData& src, SEScalarMassPerMass& dst);
  static void Serialize(const SEScalarMassPerMass& src, CDM_BIND::ScalarMassPerMassData& dst);

  static void Load(const CDM_BIND::ScalarMassPerTimeData& src, SEScalarMassPerTime& dst);
  static CDM_BIND::ScalarMassPerTimeData* Unload(const SEScalarMassPerTime& src);
  static void Serialize(const CDM_BIND::ScalarMassPerTimeData& src, SEScalarMassPerTime& dst);
  static void Serialize(const SEScalarMassPerTime& src, CDM_BIND::ScalarMassPerTimeData& dst);

  static void Load(const CDM_BIND::ScalarMassPerVolumeData& src, SEScalarMassPerVolume& dst);
  static CDM_BIND::ScalarMassPerVolumeData* Unload(const SEScalarMassPerVolume& src);
  static void Serialize(const CDM_BIND::ScalarMassPerVolumeData& src, SEScalarMassPerVolume& dst);
  static void Serialize(const SEScalarMassPerVolume& src, CDM_BIND::ScalarMassPerVolumeData& dst);

  static void Load(const CDM_BIND::ScalarNegative1To1Data& src, SEScalarNegative1To1& dst);
  static CDM_BIND::ScalarNegative1To1Data* Unload(const SEScalarNegative1To1& src);
  static void Serialize(const CDM_BIND::ScalarNegative1To1Data& src, SEScalarNegative1To1& dst);
  static void Serialize(const SEScalarNegative1To1& src, CDM_BIND::ScalarNegative1To1Data& dst);

  static void Load(const CDM_BIND::ScalarOsmolalityData& src, SEScalarOsmolality& dst);
  static CDM_BIND::ScalarOsmolalityData* Unload(const SEScalarOsmolality& src);
  static void Serialize(const CDM_BIND::ScalarOsmolalityData& src, SEScalarOsmolality& dst);
  static void Serialize(const SEScalarOsmolality& src, CDM_BIND::ScalarOsmolalityData& dst);

  static void Load(const CDM_BIND::ScalarOsmolarityData& src, SEScalarOsmolarity& dst);
  static CDM_BIND::ScalarOsmolarityData* Unload(const SEScalarOsmolarity& src);
  static void Serialize(const CDM_BIND::ScalarOsmolarityData& src, SEScalarOsmolarity& dst);
  static void Serialize(const SEScalarOsmolarity& src, CDM_BIND::ScalarOsmolarityData& dst);

  static void Load(const CDM_BIND::ScalarPowerData& src, SEScalarPower& dst);
  static CDM_BIND::ScalarPowerData* Unload(const SEScalarPower& src);
  static void Serialize(const CDM_BIND::ScalarPowerData& src, SEScalarPower& dst);
  static void Serialize(const SEScalarPower& src, CDM_BIND::ScalarPowerData& dst);

  static void Load(const CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData& src, SEScalarPowerPerAreaTemperatureToTheFourth& dst);
  static CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData* Unload(const SEScalarPowerPerAreaTemperatureToTheFourth& src);
  static void Serialize(const CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData& src, SEScalarPowerPerAreaTemperatureToTheFourth& dst);
  static void Serialize(const SEScalarPowerPerAreaTemperatureToTheFourth& src, CDM_BIND::ScalarPowerPerAreaTemperatureToTheFourthData& dst);

  static void Load(const CDM_BIND::ScalarPressureData& src, SEScalarPressure& dst);
  static CDM_BIND::ScalarPressureData* Unload(const SEScalarPressure& src);
  static void Serialize(const CDM_BIND::ScalarPressureData& src, SEScalarPressure& dst);
  static void Serialize(const SEScalarPressure& src, CDM_BIND::ScalarPressureData& dst);

  static void Load(const CDM_BIND::ScalarPressurePerVolumeData& src, SEScalarPressurePerVolume& dst);
  static CDM_BIND::ScalarPressurePerVolumeData* Unload(const SEScalarPressurePerVolume& src);
  static void Serialize(const CDM_BIND::ScalarPressurePerVolumeData& src, SEScalarPressurePerVolume& dst);
  static void Serialize(const SEScalarPressurePerVolume& src, CDM_BIND::ScalarPressurePerVolumeData& dst);

  static void Load(const CDM_BIND::ScalarPressureTimePerAreaData& src, SEScalarPressureTimePerArea& dst);
  static CDM_BIND::ScalarPressureTimePerAreaData* Unload(const SEScalarPressureTimePerArea& src);
  static void Serialize(const CDM_BIND::ScalarPressureTimePerAreaData& src, SEScalarPressureTimePerArea& dst);
  static void Serialize(const SEScalarPressureTimePerArea& src, CDM_BIND::ScalarPressureTimePerAreaData& dst);

  static void Load(const CDM_BIND::ScalarPressureTimePerVolumeData& src, SEScalarPressureTimePerVolume& dst);
  static CDM_BIND::ScalarPressureTimePerVolumeData* Unload(const SEScalarPressureTimePerVolume& src);
  static void Serialize(const CDM_BIND::ScalarPressureTimePerVolumeData& src, SEScalarPressureTimePerVolume& dst);
  static void Serialize(const SEScalarPressureTimePerVolume& src, CDM_BIND::ScalarPressureTimePerVolumeData& dst);

  static void Load(const CDM_BIND::ScalarPressureTimePerVolumeAreaData& src, SEScalarPressureTimePerVolumeArea& dst);
  static CDM_BIND::ScalarPressureTimePerVolumeAreaData* Unload(const SEScalarPressureTimePerVolumeArea& src);
  static void Serialize(const CDM_BIND::ScalarPressureTimePerVolumeAreaData& src, SEScalarPressureTimePerVolumeArea& dst);
  static void Serialize(const SEScalarPressureTimePerVolumeArea& src, CDM_BIND::ScalarPressureTimePerVolumeAreaData& dst);

  static void Load(const CDM_BIND::ScalarPressureTimeSquaredPerVolumeData& src, SEScalarPressureTimeSquaredPerVolume& dst);
  static CDM_BIND::ScalarPressureTimeSquaredPerVolumeData* Unload(const SEScalarPressureTimeSquaredPerVolume& src);
  static void Serialize(const CDM_BIND::ScalarPressureTimeSquaredPerVolumeData& src, SEScalarPressureTimeSquaredPerVolume& dst);
  static void Serialize(const SEScalarPressureTimeSquaredPerVolume& src, CDM_BIND::ScalarPressureTimeSquaredPerVolumeData& dst);

  static void Load(const CDM_BIND::ScalarTemperatureData& src, SEScalarTemperature& dst);
  static CDM_BIND::ScalarTemperatureData* Unload(const SEScalarTemperature& src);
  static void Serialize(const CDM_BIND::ScalarTemperatureData& src, SEScalarTemperature& dst);
  static void Serialize(const SEScalarTemperature& src, CDM_BIND::ScalarTemperatureData& dst);

  static void Load(const CDM_BIND::ScalarTimeData& src, SEScalarTime& dst);
  static CDM_BIND::ScalarTimeData* Unload(const SEScalarTime& src);
  static void Serialize(const CDM_BIND::ScalarTimeData& src, SEScalarTime& dst);
  static void Serialize(const SEScalarTime& src, CDM_BIND::ScalarTimeData& dst);

  static void Load(const CDM_BIND::ScalarUnsignedData& src, SEScalarUnsigned& dst);
  static CDM_BIND::ScalarUnsignedData* Unload(const SEScalarUnsigned& src);
  static void Serialize(const CDM_BIND::ScalarUnsignedData& src, SEScalarUnsigned& dst);
  static void Serialize(const SEScalarUnsigned& src, CDM_BIND::ScalarUnsignedData& dst);

  static void Load(const CDM_BIND::ScalarVolumeData& src, SEScalarVolume& dst);
  static CDM_BIND::ScalarVolumeData* Unload(const SEScalarVolume& src);
  static void Serialize(const CDM_BIND::ScalarVolumeData& src, SEScalarVolume& dst);
  static void Serialize(const SEScalarVolume& src, CDM_BIND::ScalarVolumeData& dst);

  static void Load(const CDM_BIND::ScalarVolumePerPressureData& src, SEScalarVolumePerPressure& dst);
  static CDM_BIND::ScalarVolumePerPressureData* Unload(const SEScalarVolumePerPressure& src);
  static void Serialize(const CDM_BIND::ScalarVolumePerPressureData& src, SEScalarVolumePerPressure& dst);
  static void Serialize(const SEScalarVolumePerPressure& src, CDM_BIND::ScalarVolumePerPressureData& dst);

  static void Load(const CDM_BIND::ScalarVolumePerTimeData& src, SEScalarVolumePerTime& dst);
  static CDM_BIND::ScalarVolumePerTimeData* Unload(const SEScalarVolumePerTime& src);
  static void Serialize(const CDM_BIND::ScalarVolumePerTimeData& src, SEScalarVolumePerTime& dst);
  static void Serialize(const SEScalarVolumePerTime& src, CDM_BIND::ScalarVolumePerTimeData& dst);

  static void Load(const CDM_BIND::ScalarVolumePerTimeAreaData& src, SEScalarVolumePerTimeArea& dst);
  static CDM_BIND::ScalarVolumePerTimeAreaData* Unload(const SEScalarVolumePerTimeArea& src);
  static void Serialize(const CDM_BIND::ScalarVolumePerTimeAreaData& src, SEScalarVolumePerTimeArea& dst);
  static void Serialize(const SEScalarVolumePerTimeArea& src, CDM_BIND::ScalarVolumePerTimeAreaData& dst);

  static void Load(const CDM_BIND::ScalarVolumePerTimeMassData& src, SEScalarVolumePerTimeMass& dst);
  static CDM_BIND::ScalarVolumePerTimeMassData* Unload(const SEScalarVolumePerTimeMass& src);
  static void Serialize(const CDM_BIND::ScalarVolumePerTimeMassData& src, SEScalarVolumePerTimeMass& dst);
  static void Serialize(const SEScalarVolumePerTimeMass& src, CDM_BIND::ScalarVolumePerTimeMassData& dst);

  static void Load(const CDM_BIND::ScalarVolumePerTimePressureData& src, SEScalarVolumePerTimePressure& dst);
  static CDM_BIND::ScalarVolumePerTimePressureData* Unload(const SEScalarVolumePerTimePressure& src);
  static void Serialize(const CDM_BIND::ScalarVolumePerTimePressureData& src, SEScalarVolumePerTimePressure& dst);
  static void Serialize(const SEScalarVolumePerTimePressure& src, CDM_BIND::ScalarVolumePerTimePressureData& dst);

  static void Load(const CDM_BIND::ScalarVolumePerTimePressureAreaData& src, SEScalarVolumePerTimePressureArea& dst);
  static CDM_BIND::ScalarVolumePerTimePressureAreaData* Unload(const SEScalarVolumePerTimePressureArea& src);
  static void Serialize(const CDM_BIND::ScalarVolumePerTimePressureAreaData& src, SEScalarVolumePerTimePressureArea& dst);
  static void Serialize(const SEScalarVolumePerTimePressureArea& src, CDM_BIND::ScalarVolumePerTimePressureAreaData& dst);

  // Arrays
  static void Load(const CDM_BIND::ArrayData& src, SEArray& dst);
  static CDM_BIND::ArrayData* Unload(const SEArray& src);
  static void Serialize(const CDM_BIND::ArrayData& src, SEArray& dst);
  static void Serialize(const SEArray& src, CDM_BIND::ArrayData& dst);

  static void Load(const CDM_BIND::ArrayElectricPotentialData& src, SEArrayElectricPotential& dst);
  static CDM_BIND::ArrayElectricPotentialData* Unload(const SEArrayElectricPotential& src);
  static void Serialize(const CDM_BIND::ArrayElectricPotentialData& src, SEArrayElectricPotential& dst);
  static void Serialize(const SEArrayElectricPotential& src, CDM_BIND::ArrayElectricPotentialData& dst);

  // Functions
  static void Load(const CDM_BIND::FunctionData& src, SEFunction& dst);
  static CDM_BIND::FunctionData* Unload(const SEFunction& src);
  static void Serialize(const CDM_BIND::FunctionData& src, SEFunction& dst);
  static void Serialize(const SEFunction& src, CDM_BIND::FunctionData& dst);

  static void Load(const CDM_BIND::FunctionElectricPotentialVsTimeData& src, SEFunctionElectricPotentialVsTime& dst);
  static CDM_BIND::FunctionElectricPotentialVsTimeData* Unload(const SEFunctionElectricPotentialVsTime& src);
  static void Serialize(const CDM_BIND::FunctionElectricPotentialVsTimeData& src, SEFunctionElectricPotentialVsTime& dst);
  static void Serialize(const SEFunctionElectricPotentialVsTime& src, CDM_BIND::FunctionElectricPotentialVsTimeData& dst);

  static void Load(const CDM_BIND::FunctionVolumeVsTimeData& src, SEFunctionVolumeVsTime& dst);
  static CDM_BIND::FunctionVolumeVsTimeData* Unload(const SEFunctionVolumeVsTime& src);
  static void Serialize(const CDM_BIND::FunctionVolumeVsTimeData& src, SEFunctionVolumeVsTime& dst);
  static void Serialize(const SEFunctionVolumeVsTime& src, CDM_BIND::FunctionVolumeVsTimeData& dst);

  // Histogram
  static void Load(const CDM_BIND::HistogramData& src, SEHistogram& dst);
  static CDM_BIND::HistogramData* Unload(const SEHistogram& src);
  static void Serialize(const CDM_BIND::HistogramData& src, SEHistogram& dst);
  static void Serialize(const SEHistogram& src, CDM_BIND::HistogramData& dst);

  static void Load(const CDM_BIND::HistogramFractionVsLengthData& src, SEHistogramFractionVsLength& dst);
  static CDM_BIND::HistogramFractionVsLengthData* Unload(const SEHistogramFractionVsLength& src);
  static void Serialize(const CDM_BIND::HistogramFractionVsLengthData& src, SEHistogramFractionVsLength& dst);
  static void Serialize(const SEHistogramFractionVsLength& src, CDM_BIND::HistogramFractionVsLengthData& dst);

  // Running Average
  static void Load(const CDM_BIND::RunningAverageData& src, SERunningAverage& dst);
  static CDM_BIND::RunningAverageData* Unload(const SERunningAverage& src);
  static void Serialize(const CDM_BIND::RunningAverageData& src, SERunningAverage& dst);
  static void Serialize(const SERunningAverage& src, CDM_BIND::RunningAverageData& dst);

  // Temporal Interpolator
  static void Load(const CDM_BIND::TemporalInterpolatorData& src, SETemporalInterpolator& dst);
  static CDM_BIND::TemporalInterpolatorData* Unload(const SETemporalInterpolator& src);
  static void Serialize(const CDM_BIND::TemporalInterpolatorData& src, SETemporalInterpolator& dst);
  static void Serialize(const SETemporalInterpolator& src, CDM_BIND::TemporalInterpolatorData& dst);

};
