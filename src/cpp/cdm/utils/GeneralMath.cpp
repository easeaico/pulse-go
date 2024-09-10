/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include <sstream>
#include "cdm/utils/GeneralMath.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarInversePressure.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarMassPerAmount.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarOsmolarity.h"
#include "cdm/properties/SEScalarOsmolality.h"

PUSH_EIGEN_WARNINGS
#include "unsupported/Eigen/Splines"
POP_EIGEN_WARNINGS

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the concentration of a substance given mass and volume. concentration = mass / volume
//--------------------------------------------------------------------------------------------------
bool GeneralMath::CalculateConcentration(const SEScalarMass& mass, const SEScalarVolume& volume, SEScalarMassPerVolume& concentration, Logger* logger)
{
  bool ret = true;
  //concentration = mass/volume
  double mass_ug = mass.GetValue(MassUnit::ug);
  double  volume_mL = volume.GetValue(VolumeUnit::mL);
  //Can't have a negative mass or volume
  if (mass_ug < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateConcentration: Mass is negative:" << mass_ug << " ug. Setting concentration to 0.";
    logger->Warning(ss);
    mass_ug = 0.0;
    ret = false;
  }
  if (volume_mL < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateConcentration: Volume is negative:" << volume_mL << " mL. Setting it to 0.";
    logger->Warning(ss);
    volume_mL = 0.0;
    ret = false;
  }
  if (SEScalar::IsZero(volume_mL,ZERO_APPROX))
    concentration.SetValue(0.0, MassPerVolumeUnit::ug_Per_mL);
  else
    concentration.SetValue(mass_ug / volume_mL, MassPerVolumeUnit::ug_Per_mL);
  return ret;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the mass of a substance given concentration and volume. mass = concentration * volume
//--------------------------------------------------------------------------------------------------
bool GeneralMath::CalculateMass(const SEScalarVolume& volume, const SEScalarMassPerVolume& concentration, SEScalarMass& mass, Logger* logger)
{
  bool ret = true;
  // mass = concentration*volume
  double volume_mL = volume.GetValue(VolumeUnit::mL);
  double concentration_ug_Per_mL = concentration.GetValue(MassPerVolumeUnit::ug_Per_mL);
  if (concentration_ug_Per_mL < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateMass: Mass is negative:" << concentration_ug_Per_mL << " ug/mL. Setting it to 0.";
    logger->Warning(ss);
    concentration_ug_Per_mL = 0.0;
    ret = false;
  }
  if (volume_mL < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateMass: Volume is negative:" << volume_mL << " mL. Setting it to 0.";
    logger->Warning(ss);
    volume_mL = 0.0;
    ret = false;
  }
  mass.SetValue(volume_mL*concentration_ug_Per_mL, MassUnit::ug);
  return ret;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the mass of a substance given concentration and volume. mass = concentration * volume
//--------------------------------------------------------------------------------------------------
bool GeneralMath::CalculateHenrysLawConcentration(const SESubstance& substance, const SEScalarPressure& partialPressure, SEScalarMassPerVolume& concentration, Logger* logger)
{
  bool ret = true;
  double pp_mmHg = partialPressure.GetValue(PressureUnit::mmHg);
  if (substance.GetState() != eSubstance_State::Gas)
    throw CommonDataModelException("Cannot calculate a molarity by Henry's law from partial pressure of a non gaseous substance in a liquid");
  if (pp_mmHg < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateHenrysLawConcentration: Pressure is negative:" << pp_mmHg << " mmHg. Setting it to 0.";
    logger->Warning(ss);
    pp_mmHg = 0.0;
    ret = false;
  }
  double density_ug_Per_mL = substance.GetDensity(MassPerVolumeUnit::ug_Per_mL);
  double solubilityCoefficient_Per_mmHg = substance.GetSolubilityCoefficient(InversePressureUnit::Inverse_mmHg);
  double concentration_ug_Per_mL = pp_mmHg * density_ug_Per_mL * solubilityCoefficient_Per_mmHg;
  concentration.SetValue(concentration_ug_Per_mL, MassPerVolumeUnit::ug_Per_mL);
  return ret;
}


///\cite Bhagat1984CalculatedOsmolality
void GeneralMath::CalculateOsmolarity(const SEScalarAmountPerVolume& sodiumMolarity, const SEScalarAmountPerVolume& potassiumMolarity, const SEScalarAmountPerVolume& glucoseMolarity, const SEScalarAmountPerVolume& ureaMolarity, SEScalarOsmolarity& fluidOsmolarity)
{
  double osmolarity_mOsm_Per_L = 1.89 * sodiumMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.38 * potassiumMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.08 * glucoseMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.03 * ureaMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 7.45;
  fluidOsmolarity.SetValue(osmolarity_mOsm_Per_L, OsmolarityUnit::mOsm_Per_L);
}

///\cite Bhagat1984CalculatedOsmolality
void GeneralMath::CalculateOsmolality(const SEScalarAmountPerVolume& sodiumMolarity, const SEScalarAmountPerVolume& potassiumMolarity, const SEScalarAmountPerVolume& glucoseMolarity, const SEScalarAmountPerVolume& ureaMolarity, const SEScalar& specificGravity, SEScalarOsmolality& fluidOsmolality)
{
  double waterDensity_g_mL = 0.993;
  double fluidDensity_kg_L = specificGravity.GetValue() * waterDensity_g_mL ;
  double osmolarity_mOsm_Per_L = (1.89 * sodiumMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.38 * potassiumMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.08 * glucoseMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 1.03 * ureaMolarity.GetValue(AmountPerVolumeUnit::mmol_Per_L) + 7.45);

  // Convert to kilograms by dividing by density of the fluid:
  double osmolality_mOsm_Per_kg = osmolarity_mOsm_Per_L / fluidDensity_kg_L;
  fluidOsmolality.SetValue(osmolality_mOsm_Per_kg, OsmolalityUnit::mOsm_Per_kg);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the partial pressure of a substance in gas based on the volumeFraction * pressure
//--------------------------------------------------------------------------------------------------
bool GeneralMath::CalculatePartialPressureInGas(const SEScalar0To1& volumeFraction, const SEScalarPressure& pressure, SEScalarPressure& partialPressure, Logger* logger)
{
  bool ret = true;
  double VolumeFraction = volumeFraction.GetValue();
  double pressure_cmH2O = pressure.GetValue(PressureUnit::cmH2O);
  if (VolumeFraction < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateConcentration: Volume Fraction is negative:" << VolumeFraction << ". Setting it to 0.";
    logger->Warning(ss);
    VolumeFraction = 0.0;
    ret = false;
  }
  partialPressure.SetValue(VolumeFraction*pressure_cmH2O, PressureUnit::cmH2O);
  return ret;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the partial pressure of a substance in liquid using (density and solubility coefficient) and the concentration
//--------------------------------------------------------------------------------------------------
bool GeneralMath::CalculatePartialPressureInLiquid(const SESubstance& substance, const SEScalarMassPerVolume& concentration, SEScalarPressure& partialPressure, Logger* logger)
{
  bool ret = true;
  if (substance.GetState() != eSubstance_State::Gas)
    throw CommonDataModelException("Cannot calculate a partial pressure of a non gaseous substance in a liquid");
  double concentration_ug_Per_mL = concentration.GetValue(MassPerVolumeUnit::ug_Per_mL);
  if (concentration_ug_Per_mL < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateConcentration: Concentration is negative:" << concentration_ug_Per_mL << " ug/mL. Setting it to 0.";
    logger->Warning(ss);
    concentration_ug_Per_mL = 0.0;
    ret = false;
  }
  double density_ug_Per_mL = substance.GetDensity(MassPerVolumeUnit::ug_Per_mL);
  double solubilityCoefficient_Per_mmHg = substance.GetSolubilityCoefficient(InversePressureUnit::Inverse_mmHg);
  double partialPressure_mmHg = (concentration_ug_Per_mL / density_ug_Per_mL) / solubilityCoefficient_Per_mmHg;
  partialPressure.SetValue(partialPressure_mmHg, PressureUnit::mmHg);
  return ret;
}

//--------------------------------------------------------------------------------------------------
/// \brief
// This function makes the assumption that the fluid medium is water to extrapolate that the fluid volume in mL is equivalent to the fluid weight in g  
//--------------------------------------------------------------------------------------------------
bool GeneralMath::CalculateSpecificGravity(const SEScalarMass& mass, const SEScalarVolume& volume, SEScalar& specificGravity, Logger* logger)
{
  bool ret = true;
  // compute density of given substance then divide by water density in g/mL (assuming 37 degrees C, body internal temp):
  double mass_g = mass.GetValue(MassUnit::g);
  double  volume_mL = volume.GetValue(VolumeUnit::mL);
  double totalmass_g = mass_g + volume_mL;
  double waterDensity_g_mL = 0.993;
  //Can't have a negative mass or volume
  if (totalmass_g < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateSpecificGravity: Mass is negative:" << totalmass_g << " ug. Setting it to 0.";
    logger->Warning(ss);
    totalmass_g = 0.0;
    ret = false;
  }
  if (volume_mL < 0.0)
  {
    std::stringstream ss;
    ss << "GeneralMath::CalculateSpecificGravity: Volume is negative:" << volume_mL << " mL. Setting it to 0.";
    logger->Warning(ss);
    volume_mL = 0.0;
    ret = false;
  }
  if (SEScalar::IsZero(volume_mL, ZERO_APPROX))
    specificGravity.SetValue(0.0);
  else
    specificGravity.SetValue((totalmass_g / volume_mL) / waterDensity_g_mL);
  return ret;
}

double GeneralMath::PercentDifference(double expected, double calculated)
{
  if (SEScalar::IsZero(calculated, ZERO_APPROX) && SEScalar::IsZero(expected, ZERO_APPROX))
    return 0.0;

  double difference = (calculated - expected);
  double average = (calculated + expected) / 2.0;

  if (SEScalar::IsZero(average, ZERO_APPROX))
  {
    return SEScalar::dNaN();
  }
  return std::abs(difference / average) * 100.0;
}

double GeneralMath::PercentTolerance(double expected, double calculated, double epsilon)
{
  if (SEScalar::IsZero(expected, ZERO_APPROX))
  {
    if (SEScalar::IsZero(calculated, ZERO_APPROX))
      return 0.0;
    else
    {
      if (std::abs(calculated) > epsilon)
      {
        //todo: reevaluate how to handle this - it potentially causes a problem during stabilization
        //throw CommonDataModelException("Percent Tolerance is NaN");
        return 100.0;
      }
      else
        return 0.0;
    }
  }
  double err = std::abs(calculated - expected) / expected * 100.0;
  if (std::isnan(err))
  {
    if (std::isnan(expected))
      throw CommonDataModelException("Provided Expected value is NaN");
    if (std::isnan(calculated))
      throw CommonDataModelException("Provided Calculated value is NaN");
    throw CommonDataModelException("Percent Tolerance is NaN");
  }
  return err;
}

void GeneralMath::Combinations(std::vector<size_t> maxValues, std::vector<std::vector<size_t>>& permutations)
{
  size_t numVals = 1;
  size_t Olength = maxValues.size();
  std::vector<size_t> *current, *next;

  for (size_t i = 0; i < Olength; i++)
    numVals *= maxValues[i] + 1;
  permutations.clear();
  permutations.resize(numVals);
  // First is all zeros
  int perm = 0;
  current = &permutations[perm++];
  current->resize(Olength, 0);

  for (size_t iter = 0; iter < numVals - 1; iter++)
  {
    next = &permutations[perm++];
    Copy(*current, *next);
    current = next;
    for (size_t i = 0; i <= Olength; i++)
    {
      if (++(*current)[i] > maxValues[i])
      {
        (*current)[i] = 0;
      }
      else
      {
        break;
      }
    }
    //for (int c = 0; c < Olength;c++)
    //  std::cout << (*current)[c] << " ";
    //std::cout << std::endl;
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Performs a 1D linear interpolation between two points given the percent between start/finish
///
/// \param  start      initial value
/// \param  finish     target value
/// \param  percent    percent to interpolate between initial and target
///
/// \return the value between initial and target at the given percent
//--------------------------------------------------------------------------------------------------
double GeneralMath::LinearInterpolator(double initial, double target, double percent)
{
  return initial + ((target - initial) * percent);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Performs linear interpolation between two points
///
/// \param  x1      value 1 on the x axis, must be smaller than x2
/// \param  x2      value 2 on the x axis, must be larger than x1
/// \param  y1      the y value corresponding to x1
/// \param  y2      the y value corresponding to x2
/// \param  xPrime  the interpolation point
///
/// \return the y value corresponding to xPrime
///
/// \details
/// Linear Interpolator finds the y value at xPrime, as long as xPrime is between x1 and x2. The slope
/// and Y intercept of the line connecting (x1,y1) and (x2,y2) are then found so y = mx + b can be used 
/// to find yPrime by inputting xPrime.
//--------------------------------------------------------------------------------------------------
double GeneralMath::LinearInterpolator(double x1, double x2, double y1, double y2, double xPrime)
{
  double yPrime; // Initialize the output variable
  double slope;  // Initialize the variable used to determine the slope between x1 and x2
  double yInt;   // Initialize the variable used to determine the y intercept

  if (xPrime < x1 || xPrime > x2) // make sure input x is within bounds set by x1 and x2
  {
    // Linear Interpolator requires that input x is within two other data points
    yPrime = SEScalar::dNaN();
    return yPrime;
  }

  slope = (y2 - y1) / (x2 - x1); // linear slope equals dy/dx

  yInt = y1 - slope*x1;       // Y = mx + b -> b = Y - mx

  yPrime = slope*xPrime + yInt;  // Interpolate yPrime from the created line

  return yPrime;
}

double GeneralMath::PiecewiseLinearInterpolator(const std::vector<std::pair<double, double>>& points, double x)
{
  for (unsigned int i = 1; i < points.size(); ++i) 
  {
    if (x <= points[i].first)
    {
      if (i == 0)
      {
        return SEScalar::dNaN(); // return NaN if x is less than all x values in the points
      }

      double x1 = points.at(i - 1).first;
      double y1 = points.at(i - 1).second;
      double x2 = points[i].first;
      double y2 = points[i].second;
      return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
    }
  }

  return SEScalar::dNaN(); // return NaN if x is greater than all x values in the points
}


// --------------------------------------------------------------------------------------------------
/// \brief
/// Returns y value for the specified logarithmic function given a normalized x value (0.0 to 1.0).
///
///   y = dbase ^ [(Log10(dmin/dmax) * x) + Log10(dmax)]
/// 
/// \param  dbase    base value
/// \param  dmin     minimum 
/// \param  dmax     maximum
/// \param  x    normalized x, 0.0 to 1.0
///
/// \return y
///
/// \details
/// Exponential function used to model changes based on severity. Requires a base value for the function, 
/// a maximum y value (x = 0), and minimum y value (x = 1).  Return the y value that maps to x.
///
/// Limitations:
///  - dmin > 0
///  - dmax > 0 
///  - 0.0 <= dx <= 1.0
///  
/// If any of the input variables fall outside the above bounds, the function returns NaN
///
/// Input with a min value greater than max value will be exponential growth 
//--------------------------------------------------------------------------------------------------
double GeneralMath::ExponentialDecayFunction(double dbase, double dmin, double dmax, double dx)
{
  double dy = SEScalar::dNaN();     // Resistance value
  if (dmin > 0.0 && dmax > 0.0 && dx >= 0.0 && dx <= 1.0)
  {
    dy = pow(dbase, ((log10(dmin / dmax) * dx) + log10(dmax)));
  }
  return dy;
}

// --------------------------------------------------------------------------------------------------
/// \brief
/// Returns y value for the specified logarithmic function given a normalized x value (0.0 to 1.0).
///
///   y = dbase ^ [(Log10(dmax/dmin) * x) + Log10(dmin)]
/// 
/// \param  dbase    base value
/// \param  dmin     minimum 
/// \param  dmax     maximum
/// \param  x    normalized x, 0.0 to 1.0
///
/// \return y
///
/// \details
/// Exponential function used to model changes based on severity. Requires a base value for the function, 
/// a minimum y value (x = 0), and maximum y value (x = 1).  Return the y value that maps to x.
///
/// Limitations:
///  - dmin > 0
///  - dmax > 0 
///  - 0.0 <= dx <= 1.0
///  
/// If any of the input variables fall outside the above bounds, the function returns NaN
///
/// Input with a min value greater than max value will be exponential decay 
//--------------------------------------------------------------------------------------------------
double GeneralMath::ExponentialGrowthFunction(double dbase, double dmin, double dmax, double dx)
{
  double dy = SEScalar::dNaN();     // Resistance value
  if (dmax > 0.0 && dmin > 0.0 && dx >= 0.0 && dx <= 1.0)
  {
    dy = pow(dbase, ((log10(dmax / dmin) * dx) + log10(dmin)));
  }
  return dy;
}

// --------------------------------------------------------------------------------------------------
/// \brief
/// A logistic function.
/// 
/// \param  a    asymptote
/// \param  x50  x-value at sigmoid midpoint 
/// \param  k    curve steepness and direction parameter
/// \param  x     input value
///
/// \return y
///
/// \details
/// A logistic function.
//--------------------------------------------------------------------------------------------------
double GeneralMath::LogisticFunction(double a, double x50, double k, double x)
{
  double y = a / (1 + exp(-k*(x - x50)));

  return y;
}

double GeneralMath::RootMeanSquaredError(std::vector<double> observed, std::vector<double> predicted, bool normalize, double outlierStandardDeviation)
{
  double squaredSum = 0;
  double sum = 0;
  unsigned int length = 0;
  double observedMax = 0.0;
  double observedMin = 0.0;

  for (unsigned int i = 0; i < observed.size(); i++)
  {
    double difference = std::abs(observed[i] - predicted[i]);
    sum += difference;
    squaredSum += std::pow(difference, 2);
    length++;

    if (i == 0 || observed[i] > observedMax)
    {
      observedMax = observed[i];
    }
    if (i == 0 || observed[i] < observedMax)
    {
      observedMin = observed[i];
    }
  }

  if (outlierStandardDeviation > 0)
  {
    double mean = sum / length;
    double variance = 0;
    for (unsigned int i = 0; i < observed.size(); i++)
    {
      variance += std::pow(std::abs(observed[i] - predicted[i]) - mean, 2);
    }
    variance /= length;
    double standardDeviation = std::sqrt(variance);

    double maxDifference = outlierStandardDeviation * standardDeviation;
    for (unsigned int i = 0; i < observed.size(); i++)
    {
      double difference = std::abs(observed[i] - predicted[i]);
      if (difference > maxDifference)
      {
        squaredSum -= std::pow(difference, 2); // Remove it from the calculation
        length--;
        continue; // Doesn't effect normalizing
      }

      if (i == 0 || observed[i] > observedMax)
      {
        observedMax = observed[i];
      }
      if (i == 0 || observed[i] < observedMax)
      {
        observedMin = observed[i];
      }
    }
  }

  double RMSE = std::sqrt(squaredSum / length);
  if (normalize)
  {
    RMSE /= (observedMax - observedMin);
  }

  return RMSE;
}

void InsertInterpolation(std::vector<double>& v, size_t idx)
{
  // Get the mid value between where we putting our new point
  double newValue = (v[idx] + v[idx + 1]) / 2;
  v.insert(v.begin() + idx, newValue);
}
void RemoveInterpolation(std::vector<double>& v, size_t idx)
{
  // Get the mid value between the 2 points we are going to remove
  double newValue = (v[idx] + v[idx + 1]) / 2;
  // Take out idx and idx+1
  v.erase(v.begin() + idx);
  v.erase(v.begin() + idx);
  v.insert(v.begin() + idx, newValue);
}
//--------------------------------------------------------------------------------------------------
/// \brief
/// A 1D linear interpolator which keeps the min/max value from input

/// \param  v input 1D vector
/// \param  newSize  number of points in output vector

/// \details
/// A 1D linear interpolator which sets the min/max values from the original input to the nearest neighbour points
/// for maintaining the range the original input. Both input and output are assumed to be equally spaced.
//--------------------------------------------------------------------------------------------------
void GeneralMath::LinearInterpolator1(std::vector<double> &v, size_t newSize)
{
  const auto nInput = v.size();
  auto outputInterval = ((double)nInput - 1) / ((double)newSize - 1);

  // Linear interpolation
  std::vector<double> newV;
  newV.reserve(newSize);
  for (size_t i = 0; i < newSize-1; ++i)
  {
    auto currentIndex = (double)i * outputInterval;
    double x1 = std::floor(currentIndex);
    double x2 = std::floor(currentIndex + 1);
    double y1 = v[(int)x1];
    double y2 = v[(int)x2];
    double yPrime = y1 * (currentIndex - x1) + y2 * (x2 - currentIndex);
    newV.push_back(yPrime);
  }
  newV.push_back(v.back());

  // Set min/max value to the nearest points to maintain min/max value
  auto minmaxElement = std::minmax_element(v.begin(), v.end());
  auto minIndex = (int)std::round((double)(minmaxElement.first - v.begin()) / outputInterval);
  auto maxIndex = (int)std::round((double)(minmaxElement.second - v.begin()) / outputInterval);
  newV[minIndex] = *minmaxElement.first;
  newV[maxIndex] = *minmaxElement.second;

  v.clear();
  v = newV;
}
bool GeneralMath::LinearInterpolator(std::vector<double>& v, size_t newSize)
{
  if (newSize == v.size())
    return true; // Done
  if (newSize > v.size())// Expand
  {
    size_t half = (size_t)std::floor(v.size() / 2);
    size_t quarter = (size_t)std::floor(half / 2);
    // Insert middle
    InsertInterpolation(v, half);
    if (newSize == v.size())
      return true; // Done

    // Insert left middle
    InsertInterpolation(v, quarter);
    if (newSize == v.size())
      return true; // Done

    // Insert right middle
    InsertInterpolation(v, half + quarter);
    if (newSize == v.size())
      return true; // Done
  }
  else // Contract
  {
    if (newSize <= 2)
      return false;// Nothing we can do with this...

    size_t half = (size_t)std::floor(v.size() / 2);
    size_t quarter = (size_t)std::floor(half / 2);
    // Insert middle

    RemoveInterpolation(v, half);
    if (newSize == v.size())
      return true; // Done

    // Insert left middle
    RemoveInterpolation(v, quarter);
    if (newSize == v.size())
      return true; // Done

    // Insert right middle
    RemoveInterpolation(v, half + quarter);
    if (newSize == v.size())
      return true; // Done
  }
  // Recurse until equal sizes
  return LinearInterpolator(v, newSize);
}

void GeneralMath::SplineInterpolater(std::vector<double>& v, size_t newSize)
{
  // https://eigen.tuxfamily.org/dox/unsupported/structEigen_1_1SplineFitting.html
  const int nInput = (int)v.size();
  Eigen::MatrixXd points(2, nInput);
  for (int i = 0; i < nInput; ++i)
  {
    points(0, i) = (double)i / (nInput - 1);
    points(1, i) = v[i];
  }

  typedef Eigen::Spline<double, 1, 2> Spline1D;
  typedef Eigen::SplineFitting<Spline1D> SplineFitting1D;
  const auto fit = SplineFitting1D::Interpolate(points.row(1), 2, points.row(0));
  Spline1D spline(fit);

  v.clear();
  v.reserve(newSize);
  for (size_t i = 0; i < newSize; ++i)
  {
    auto point = spline((double)i / (newSize - 1));
    v.push_back(point.coeffRef(0));
  }
}

double GeneralMath::Damper(double targetValue, double previousValue, double dampenFraction_perSec, double timeStep_s)
{
  double change = targetValue - previousValue;
  change *= dampenFraction_perSec * timeStep_s;
  return previousValue + change;
}

double GeneralMath::ParbolicInterpolator(double min, double max, double factor)
{
  double a = max - min;
  return a * factor * factor + min;
}
