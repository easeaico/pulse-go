/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/properties/SEScalarUnsigned.h"

SEScalarUnsigned::SEScalarUnsigned() : SEScalar()
{

}

void SEScalarUnsigned::SetValue(double d)
{ 
  if (d < 0)
    throw CommonDataModelException("SEScalarUnsigned must be between [0,inf]");
  SEScalar::SetValue(d);
}

void SEScalarUnsigned::SetValue(double d, const NoUnit& /*unitless*/)
{ 
  SEScalarUnsigned::SetValue(d);
}

void SEScalarUnsigned::ForceValue(double d)
{
  SEScalar::ForceValue(d);
}

void SEScalarUnsigned::ForceValue(double d, const NoUnit& /*unitless*/)
{
  SEScalar::ForceValue(d);
}
