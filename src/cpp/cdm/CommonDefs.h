/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
 
#pragma once
#include "pulse_export.h"
#define CDM_DECL PULSE_EXPORT

#if (0)
#define DEBUGOUT(x) x
#else
#define DEBUGOUT(x)
#endif


#define PULSE_CDM pulse::cdm
#define CDM_BIND pulse::cdm::bind

#define CDM_BIND_DECL(type) \
  namespace pulse { namespace cdm { namespace bind { class type; }}}
#define CDM_BIND_DECL2(type) \
  class SE##type;\
  namespace pulse { namespace cdm { namespace bind { class type##Data; }}}

#if defined(_MSC_VER)
  // Modify the following defines if you have to target a platform prior to the ones specified below.
  // Refer to MSDN for the latest info on corresponding values for different platforms.
  #ifndef WINVER        // Allow use of features specific to Windows XP or later.
  #define WINVER 0x0501    // Change this to the appropriate value to target other versions of Windows.
  #endif

  #ifndef _WIN32_WINNT    // Allow use of features specific to Windows XP or later.
  #define _WIN32_WINNT 0x0501  // Change this to the appropriate value to target other versions of Windows.
  #endif

  #ifndef _WIN32_WINDOWS    // Allow use of features specific to Windows 98 or later.
  #define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
  #endif

  #ifndef _WIN32_IE      // Allow use of features specific to IE 6.0 or later.
  #define _WIN32_IE 0x0600  // Change this to the appropriate value to target other versions of IE.
  #endif

  #define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers
#endif

// See https ://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/
// When adding new warnings remember to add the DISABLE_ macro
// for all three sections MSVC, GCC/CLANG, other
#if defined(_MSC_VER)

  __pragma(warning(disable : 4251)) // Ignore needs dll-interface warnings
  //#pragma warning(disable:4100) // unreferenced formal parameter (intentional in base classes)

  #define DISABLE_WARNING_PUSH           __pragma(warning( push ))
  #define DISABLE_WARNING_POP            __pragma(warning( pop ))
  #define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

  #define PUSH_PROTO_WARNINGS \
    DISABLE_WARNING_PUSH \
    DISABLE_WARNING(4127) \
    DISABLE_WARNING(4267) \
    DISABLE_WARNING(4244)
  #define POP_PROTO_WARNINGS DISABLE_WARNING_POP

  #define PUSH_EIGEN_WARNINGS \
      DISABLE_WARNING_PUSH \
      DISABLE_WARNING(4127) \
      DISABLE_WARNING(4267) \
      DISABLE_WARNING(4244)
  #define POP_EIGEN_WARNINGS DISABLE_WARNING_POP

#elif defined(__GNUC__) || defined(__clang__)
  #define DO_PRAGMA(X) _Pragma(#X)
  #define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
  #define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop)
  #define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

  #define PUSH_PROTO_WARNINGS \
    DISABLE_WARNING_PUSH
  #define POP_PROTO_WARNINGS DISABLE_WARNING_POP

  #define PUSH_EIGEN_WARNINGS \
      DISABLE_WARNING_PUSH
  #define POP_EIGEN_WARNINGS DISABLE_WARNING_POP

#else
  #define DISABLE_WARNING_PUSH
  #define DISABLE_WARNING_POP
  #define DISABLE_WARNING

  #define PUSH_PROTO_WARNINGS
  #define POP_PROTO_WARNINGS

  #define PUSH_EIGEN_WARNINGS
  #define POP_EIGEN_WARNINGS
#endif


#define _USE_MATH_DEFINES
#include <memory>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <climits>
#include <string>
#include <math.h>
#include <vector>
#include <stack>
#include <map>
#include <set>

#ifdef _MSC_VER
#include <direct.h>
#include <unordered_map>
using namespace stdext;
#else//if (__GCC__) || (__GNUC__)
#include <unordered_map>
#endif

//Utilities
enum class eSerializationFormat { JSON = 0, BINARY, VERBOSE_JSON,TEXT };

struct CommonDataModelException : public std::runtime_error
{
  CommonDataModelException()
    : std::runtime_error( "Common Data Model Error" ) {}

  CommonDataModelException(const std::string& _Message)
    : std::runtime_error(_Message) {}
};
// Basics
#include "cdm/Macros.h"
#include "cdm/utils/Logger.h"


// General Enums
// Keep enums in sync with appropriate schema/cdm/Enums.proto file !!
//
enum class eSide { NullSide = 0, Left, Right };
extern CDM_DECL const std::string& eSide_Name(eSide m);

enum class eGate { NullGate = 0, Open, Closed };
extern CDM_DECL const std::string& eGate_Name(eGate m);

enum class eSwitch { NullSwitch = 0, Off, On };
extern CDM_DECL const std::string& eSwitch_Name(eSwitch m);

enum class eCharge { NullCharge = 0, Negative, Neutral, Positive };
extern CDM_DECL const std::string& eCharge_Name(eCharge m);

enum class eBreathState { NoBreath=0,
                          PatientInhale, PatientPause, PatientExhale,
                          EquipmentInhale, EquipmentPause, EquipmentExhale,
                          ExpiratoryHold, InspiratoryHold };
extern CDM_DECL const std::string& eBreathState_Name(eBreathState m);

enum class eDefaultType { Model = 0, Zero };
extern CDM_DECL const std::string& eDefaultType_Name(eDefaultType m);

enum class eDriverWaveform { NullDriverWaveform = 0, Square,
                                                  AscendingRamp, DescendingRamp,
                                                  ExponentialGrowth, ExponentialDecay,
                                                  SinusoidalRise, SinusoidalFall,
                                                  SigmoidalRise, SigmoidalFall };
extern CDM_DECL const std::string& eDriverWaveform_Name(eDriverWaveform m);

//
// End General Enum

// IF YOU ADD A NEW SCALAR TYPE
// PLEASE UPDATE THE METHOD IN SEProperty.cpp
// ALSO DECLARE A TEMPLATE CLASS AT THE BOTTOM OF SESCALAR.CPP

class CCompoundUnit;

// Used for overrides and modifiers, a super generic way of enumerating a list of properties
// properties are an actual scalar member variable on an object
// map<variable_name,SEScalarPair(value,unit)>
class CDM_DECL SEScalarPair
{
public:
  SEScalarPair();
  SEScalarPair(double v);
  SEScalarPair(double v, const std::string& u);
  SEScalarPair(double v, const CCompoundUnit& cc);
  ~SEScalarPair() = default;
  double value;
  std::string unit;
};
using SEScalarProperties = std::map<std::string, SEScalarPair>;


class SECurve;
class SERunningAverage;
class SEScalar; class SEUnitScalar; class SEGenericScalar; class NoUnit;
class SEScalar0To1; 
class SEScalarNegative1To1;
class SEScalarUnsigned;
class SEScalarAmount; class AmountUnit;
class SEScalarAmountPerMass; class AmountPerMassUnit;
class SEScalarAmountPerTime; class AmountPerTimeUnit;
class SEScalarAmountPerVolume; class AmountPerVolumeUnit;
class SEScalarArea; class AreaUnit;
class SEScalarAreaPerTimePressure; class AreaPerTimePressureUnit;
class SEScalarElectricCapacitance; class ElectricCapacitanceUnit;
class SEScalarElectricCharge; class ElectricChargeUnit;
class SEScalarElectricCurrent; class ElectricCurrentUnit;
class SEScalarElectricInductance; class ElectricInductanceUnit;
class SEScalarElectricPotential; class ElectricPotentialUnit;
class SEScalarElectricResistance; class ElectricResistanceUnit;
class SEScalarEnergy; class EnergyUnit;
class SEScalarEnergyPerAmount; class EnergyPerAmountUnit;
class SEScalarEnergyPerMass; class EnergyPerMassUnit;
class SEScalarEquivalentWeightPerVolume; class EquivalentWeightPerVolumeUnit;
class SEScalarForce; class ForceUnit;
class SEScalar0To1;
class SEScalarFrequency; class FrequencyUnit;
class SEScalarHeatCapacitance; class HeatCapacitanceUnit;
class SEScalarHeatCapacitancePerAmount; class HeatCapacitancePerAmountUnit;
class SEScalarHeatCapacitancePerMass; class HeatCapacitancePerMassUnit;
class SEScalarHeatConductance; class HeatConductanceUnit;
class SEScalarHeatConductancePerArea; class HeatConductancePerAreaUnit;
class SEScalarHeatResistance; class HeatResistanceUnit;
class SEScalarHeatResistanceArea; class HeatResistanceAreaUnit;
class SEScalarHeatInductance; class HeatInductanceUnit;
class SEScalarInversePressure; class InversePressureUnit; 
class SEScalarInverseVolume; class InverseVolumeUnit;
class SEScalarLength; class LengthUnit;
class SEScalarLengthPerTime; class LengthPerTimeUnit;
class SEScalarLengthPerTimePressure; class LengthPerTimePressureUnit;
class SEScalarMass; class MassUnit;
class SEScalarMassPerAmount; class MassPerAmountUnit;
class SEScalarMassPerAreaTime; class MassPerAreaTimeUnit;
class SEScalarMassPerMass; class MassPerMassUnit;
class SEScalarMassPerTime; class MassPerTimeUnit;
class SEScalarMassPerVolume; class MassPerVolumeUnit;
class SEScalarOsmolality; class OsmolalityUnit;
class SEScalarOsmolarity; class OsmolarityUnit;
class SEScalarPower; class PowerUnit;
class SEScalarPowerPerAreaTemperatureToTheFourth; class PowerPerAreaTemperatureToTheFourthUnit;
class SEScalarPressure; class PressureUnit;
class SEScalarPressurePerVolume; class PressurePerVolumeUnit;
class SEScalarPressureTimePerVolume; class PressureTimePerVolumeUnit;
class SEScalarPressureTimePerVolumeArea; class PressureTimePerVolumeAreaUnit;
class SEScalarPressureTimePerArea; class PressureTimePerAreaUnit;
class SEScalarPressureTimeSquaredPerVolume; class PressureTimeSquaredPerVolumeUnit;
class SEScalarTime; class TimeUnit;
class SEScalarTemperature; class TemperatureUnit;
class SEScalarVolume; class VolumeUnit;
class SEScalarVolumePerPressure; class VolumePerPressureUnit;
class SEScalarVolumePerTime; class VolumePerTimeUnit;
class SEScalarVolumePerTimeArea; class VolumePerTimeAreaUnit;
class SEScalarVolumePerTimePressureArea; class VolumePerTimePressureAreaUnit;
class SEScalarVolumePerTimeMass; class VolumePerTimeMassUnit;
class SEScalarVolumePerTimePressure; class VolumePerTimePressureUnit;
class SETemporalInterpolator;

class SEArray;
class SEArrayElectricPotential;

class SEFunction;
class SEFunctionVolumeVsTime;
class SEFunctionElectricPotentialVsTime;

class SEHistogram;
class SEHistogramFractionVsLength;

class SESegment;
class SESegmentConstant;
class SESegmentLinear;
class SESegmentParabolic;
class SESegmentSigmoidal;
