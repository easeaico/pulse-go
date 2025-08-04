/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/PatientActions.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEBrainInjury.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/patient/actions/SESubstanceAdministration.h"
#include "cdm/patient/actions/SESupplementalOxygen.h"

const std::string& eAirwayObstruction_ResistanceType_Name(eAirwayObstruction_ResistanceType rt)
{
  return CDM_BIND::AirwayObstructionData::eResistanceType_Name((CDM_BIND::AirwayObstructionData::eResistanceType)rt);
}

const std::string& eBrainInjury_Type_Name(eBrainInjury_Type m)
{
  return CDM_BIND::BrainInjuryData::eType_Name((CDM_BIND::BrainInjuryData::eType)m);
}

const std::string& eHemorrhage_Compartment_Name(eHemorrhage_Compartment c)
{
  return CDM_BIND::HemorrhageData::eCompartment_Name((CDM_BIND::HemorrhageData::eCompartment)c);
}
eHemorrhage_Compartment eHemorrhage_Compartment_Parse(const std::string& s)
{
  std::string scmpt = s;
  // Nix 'Vasculature' if its there
  size_t idx = s.find("Vasculature");
  if (idx != std::string::npos)
    scmpt = s.substr(0, idx);

  CDM_BIND::HemorrhageData::eCompartment cmpt;
  if (!CDM_BIND::HemorrhageData::eCompartment_Parse(scmpt, &cmpt))
    return eHemorrhage_Compartment::None;
  return (eHemorrhage_Compartment)cmpt;
}
const std::string& eHemorrhage_Type_Name(eHemorrhage_Type t)
{
  return CDM_BIND::HemorrhageData::eCompartment_Name((CDM_BIND::HemorrhageData::eCompartment)t);
}

const std::string& eIntubation_Type_Name(eIntubation_Type t)
{
  return CDM_BIND::IntubationData::eType_Name((CDM_BIND::IntubationData::eType)t);
}

const std::string& eSubstanceAdministration_Route_Name(eSubstanceAdministration_Route r)
{
  return CDM_BIND::SubstanceBolusData::eRoute_Name((CDM_BIND::SubstanceBolusData::eRoute)r);
}

const std::string& eSupplementalOxygen_Device_Name(eSupplementalOxygen_Device n)
{
  return CDM_BIND::SupplementalOxygenData::eDevice_Name((CDM_BIND::SupplementalOxygenData::eDevice)n);
}