/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEDataRequest.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/PhysiologyEngine.h"
// Compartments
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartmentLink.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/compartment/thermal/SEThermalCompartment.h"
#include "cdm/compartment/tissue/SETissueCompartment.h"
// Circuit
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/circuit/thermal/SEThermalCircuit.h"
#include "cdm/circuit/thermal/SEThermalCircuitNode.h"
#include "cdm/circuit/thermal/SEThermalCircuitPath.h"
#include "cdm/circuit/electrical/SEElectricalCircuit.h"
#include "cdm/circuit/electrical/SEElectricalCircuitNode.h"
#include "cdm/circuit/electrical/SEElectricalCircuitPath.h"
// Substances
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstancePharmacokinetics.h"
#include "cdm/substance/SESubstanceTissuePharmacokinetics.h"
#include "cdm/substance/SESubstanceManager.h"
// Patient
#include "cdm/patient/SEPatient.h"
// Systems
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEDrugSystem.h"
#include "cdm/system/physiology/SEEndocrineSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SEGastrointestinalSystem.h"
#include "cdm/system/physiology/SENervousSystem.h"
#include "cdm/system/physiology/SERenalSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/system/physiology/SETissueSystem.h"
#include "cdm/system/environment/SEEnvironment.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachine.h"
#include "cdm/system/equipment/bag_valve_mask/SEBagValveMask.h"
#include "cdm/system/equipment/ecmo/SEECMO.h"
#include "cdm/system/equipment/electrocardiogram/SEElectroCardioGram.h"
#include "cdm/system/equipment/inhaler/SEInhaler.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
// Scalars
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarElectricPotential.h"
#include "cdm/utils/DataTrack.h"

SEEngineTracker::SEEngineTracker(Logger* logger) : SEDataRequestTracker(logger)
{
  m_LastPullTime_s = SEScalar::dNaN();
  m_CurrentSampleTime_s = 0;

  m_DataTrack = new DataTrack(logger);
  m_DataRequestMgr = new SEDataRequestManager(logger);
  Clear();

  m_Patient = nullptr;
}

SEEngineTracker::~SEEngineTracker()
{
  Clear();
  delete m_DataTrack;
  delete m_DataRequestMgr;

  for (auto& pair : m_OutputFiles)
  {
    if (pair.second.is_open())
      pair.second.close();
    pair.second.clear();
  }
}

void SEEngineTracker::Clear()
{
  Reset();
  m_Patient = nullptr;
  m_ActionMgr = nullptr;
  m_SubMgr = nullptr;
  m_CmptMgr = nullptr;

  m_Environment = nullptr;
  m_PhysiologySystems.clear();
  m_AnesthesiaMachine = nullptr;
  m_BVM = nullptr;
  m_ECG = nullptr;
  m_ECMO = nullptr;
  m_Inhaler = nullptr;
  m_MechanicalVentilator = nullptr;
}

void SEEngineTracker::Reset()
{
  m_Mode = TrackMode::Dynamic;
  m_ResultsStream = nullptr;

  m_DataRequestMgr->Clear();
  m_DataTrack->Clear();

  DELETE_MAP_SECOND(m_Request2Scalar);
}

void SEEngineTracker::CloseResultsFile()
{
  m_Mode = TrackMode::Dynamic;
  if (m_ResultsStream)
    m_ResultsStream->close();
}

void SEEngineTracker::SetPatient(SEPatient& p)
{
  m_Patient = &p;
}

void SEEngineTracker::SetEnvironment(SEEnvironment& e)
{
  m_Environment = &e;
}

void SEEngineTracker::AddPhysiologySystem(SESystem& sys)
{
  m_PhysiologySystems.push_back(&sys);
}

void SEEngineTracker::SetAnesthesiaMachine(SEAnesthesiaMachine& am)
{
  m_AnesthesiaMachine = &am;
}

void SEEngineTracker::SetBagValveMask(SEBagValveMask& bvm)
{
  m_BVM = &bvm;
}

void SEEngineTracker::SetElectroCardioGram(SEElectroCardioGram& ecg)
{
  m_ECG = &ecg;
}

void SEEngineTracker::SetECMO(SEECMO& ecmo)
{
  m_ECMO = &ecmo;
}

void SEEngineTracker::SetInhaler(SEInhaler& i)
{
  m_Inhaler = &i;
}

void SEEngineTracker::SetMechanicalVentilator(SEMechanicalVentilator& mv)
{
  m_MechanicalVentilator = &mv;
}

void SEEngineTracker::SetActionManager(SEActionManager& am)
{
  m_ActionMgr = &am;
}

void SEEngineTracker::SetCompartmentManager(SECompartmentManager& cMgr)
{
  m_CmptMgr = &cMgr;
}

void SEEngineTracker::SetSubstanceManager(SESubstanceManager& sMgr)
{
  m_SubMgr = &sMgr;
}

size_t SEEngineTracker::NumTracks() const
{
  return m_DataTrack->NumTracks();
}

double SEEngineTracker::GetValue(size_t idx) const
{
  return m_DataTrack->GetProbe(idx);
}

double SEEngineTracker::GetValue(const SEDataRequest& dr) const
{
  auto drs = GetScalar(dr);
  if (drs == nullptr)
    return SEScalar::dNaN();
  if (!drs->IsValid())
    return SEScalar::dNaN();
  if (dr.HasUnit())
    return drs->GetValue(*dr.GetUnit());
  return drs->GetValue();
}

std::string SEEngineTracker::GetUnit(const SEDataRequest& dr) const
{
  auto drs = GetScalar(dr);
  if (drs == nullptr)
    return "";
  if (!drs->IsValid())
    return "";
  if (dr.HasUnit())
    return dr.GetUnit()->GetString();
  if (drs->HasUnit())
    return drs->GetUnit()->GetString();
  return "";
}

void SEEngineTracker::LogRequestedValues() const
{
  SEDataRequestScalar* ds;
  for (SEDataRequest* dr : m_DataRequestMgr->GetDataRequests())
  {
    auto itr = m_Request2Scalar.find(dr);
    if (itr == m_Request2Scalar.end())
    {
      Error("Data request has no scalar...");
      continue;
    }

    ds = itr->second;
    if(!ds->IsValid())
      Info(ds->Heading + " NaN");
    else
    {
      if(!dr->HasUnit())
        Info(ds->Heading + " " + pulse::cdm::to_string(ds->GetValue()));
      else
        Info(ds->Heading + " " + pulse::cdm::to_string(ds->GetValue(*dr->GetUnit())));
    }
  }
}

bool SEEngineTracker::SetupDataRequests(const SEDataRequestManager& drMgr)
{
  Reset();
  m_DataRequestMgr->Copy(drMgr);
  if (m_DataRequestMgr->HasResultsFilename())
    m_Mode = TrackMode::CSV;

  bool success = true;

  // Process/Hook up all requests with their associated scalers
  DELETE_MAP_SECOND(m_Request2Scalar);// Get our scalars again
  for (SEDataRequest* dr : m_DataRequestMgr->GetDataRequests())
  {
    if (!ConnectRequest(*dr))
    {// Could not hook this up, get rid of it
      m_ss << "Unable to find data for " << m_Request2Scalar[dr]->Heading;
      Error(m_ss);
      success = false;
    }
  }

  // Check to see if there are any repeats in the data request manager
  if (m_DataTrack->NumTracks() != m_DataRequestMgr->GetDataRequests().size())
  {
    Warning("Number of data requests does not match the number of tracked properties!");
    Warning("--Check to see if you have duplicates in your data request list");
    Warning("--Here is the order of the data items I am traking:");
    for (size_t i = 0; i < m_DataTrack->NumTracks(); i++)
      Warning("--  " + m_DataTrack->GetProbeName(i));
    Warning("--Here is what you requested:");
    for (SEDataRequest const* dr : m_DataRequestMgr->GetDataRequests())
      Warning("--  " + dr->GetHeaderName());
    Warning("I don't have the logic to figure out which tracked items are duplicated and where they go in the pulled data array");
    Warning("If you are using C#/Java/Python and see this, your data array order is probably not what you are expecting it to be.");
    Warning("CSV files will not have duplicate columns.");
    success = false;
  }

  if (m_Mode == TrackMode::CSV)
  {
    m_ResultsStream = &m_OutputFiles[m_DataRequestMgr->GetResultFilename()];
    if (!m_ResultsStream->is_open())
    {
      m_CurrentSampleTime_s = 0;
      m_LastPullTime_s = SEScalar::dNaN();
      Info("Creating csv request file: " + m_DataRequestMgr->GetResultFilename());
      m_DataTrack->CreateFile(m_DataRequestMgr->GetResultFilename().c_str(), *m_ResultsStream);
    }
    for (auto& pair : m_OutputFiles)
    {
      // CLose out other streams we may have
      if (pair.first == m_DataRequestMgr->GetResultFilename())
        continue;
      if (pair.second.is_open())
        pair.second.close();
      pair.second.clear();
    }
  }

  return success;
}

bool SEEngineTracker::ConnectRequest(SEDataRequest& dr)
{
  if (m_Request2Scalar.find(&dr) != m_Request2Scalar.end())
  {
    return true; // We have this connected already
  }

  SEDataRequestScalar* ds = new SEDataRequestScalar(GetLogger());
  m_Request2Scalar[&dr] = ds;

  bool success = ConnectRequest(dr, *ds);

  std::string header = dr.GetHeaderName();
  if (header.empty())
  {
    m_ss << "Unhandled data request : " << dr.GetPropertyName() << std::endl;
    Error(m_ss);
    return false;
  }

  ds->Heading = header;
  ds->idx = m_DataTrack->Probe(ds->Heading, 0);
  m_DataTrack->SetFormatting(ds->Heading, dr);
  return success;
}

bool SEEngineTracker::ConnectRequest(SEDataRequest& dr, SEDataRequestScalar& ds)
{
  const SEScalar* s = nullptr;
  std::string propertyName = dr.GetPropertyName();
  switch (dr.GetCategory())
  {
    case eDataRequest_Category::Patient:
    {
      s = m_Patient->GetScalar(propertyName);
      break;
    }
    case eDataRequest_Category::Physiology:
    {
      s = SESystem::GetScalar(propertyName, &m_PhysiologySystems);
      break;
    }    
    case eDataRequest_Category::Environment:
    {
      if (m_Environment != nullptr)
        s = m_Environment->GetScalar(propertyName);
      else
        Error("Cannot track environment data as no environment was provide");
      break;
    }
    case eDataRequest_Category::Action:
    {
      s = m_ActionMgr->GetScalar(dr.GetActionName(), dr.GetCompartmentName(), dr.GetSubstanceName(), propertyName);
      break;
    }
    case eDataRequest_Category::AnesthesiaMachine:
    {
      if (m_AnesthesiaMachine != nullptr)
        s = m_AnesthesiaMachine->GetScalar(propertyName);
      else
        Error("Cannot track anesthesia machine data as no anesthesia machine was provided");
      break;
    }
    case eDataRequest_Category::BagValveMask:
    {
      if (m_BVM != nullptr)
        s = m_BVM->GetScalar(propertyName);
      else
        Error("Cannot track bag valve mask data as no bag valve mask was provided");
      break;
    }
    case eDataRequest_Category::ECG:
    {
      if (m_ECG != nullptr)
        s = m_ECG->GetScalar(propertyName);
      else
        Error("Cannot track ECG data as no ECG was provided");
      break;
    }
    case eDataRequest_Category::ECMO:
    {
      if (m_ECMO != nullptr)
        s = m_ECMO->GetScalar(propertyName);
      else
        Error("Cannot track ECMO data as no ECMO was provided");
      break;
    }
    case eDataRequest_Category::Inhaler:
    {
      if (m_Inhaler != nullptr)
        s = m_Inhaler->GetScalar(propertyName);
      else
        Error("Cannot track inhaler data as no inhaler was provided");
      break;
    }
    case eDataRequest_Category::MechanicalVentilator:
    {
      if (m_MechanicalVentilator != nullptr)
        s = m_MechanicalVentilator->GetScalar(propertyName);
      else
        Error("Cannot track mechanical ventilator data as no mechanical ventilator was provided");
      break;
    }
    case eDataRequest_Category::GasCompartment:
    {
      if (!m_CmptMgr->HasGasCompartment(dr.GetCompartmentName()))
      {
        Error("Unknown gas compartment : " + dr.GetCompartmentName());
        return false;
      }
      // Removing const because I need to create objects in order to track those objects
      SEGasCompartment* gasCmpt = (SEGasCompartment*)m_CmptMgr->GetGasCompartment(dr.GetCompartmentName());
      if (dr.HasSubstanceName())
      {
        SESubstance* sub = m_SubMgr->GetSubstance(dr.GetSubstanceName());
        // Activate this substance so compartments have it
        m_SubMgr->AddActiveSubstance(*sub);
        if (gasCmpt->HasChildren())
        {
          if (propertyName == "Volume")
            ds.UpdateProperty = CompartmentUpdate::Volume;
          else if (propertyName == "VolumeFraction")
            ds.UpdateProperty = CompartmentUpdate::VolumeFraction;
          else if (propertyName == "PartialPressure")
            ds.UpdateProperty = CompartmentUpdate::PartialPressure;
          ds.GasSubstance = gasCmpt->GetSubstanceQuantity(*sub);
        }
        s = gasCmpt->GetSubstanceQuantity(*sub)->GetScalar(propertyName);
      }
      else
      {
        if (gasCmpt->HasChildren() || gasCmpt->HasNodeMapping())
        {
          if (propertyName == "Volume")
            ds.UpdateProperty = CompartmentUpdate::Volume;
          if (propertyName == "Pressure")
            ds.UpdateProperty = CompartmentUpdate::Pressure;
        }

        {// Always Update these
          if (propertyName == "Inflow")
            ds.UpdateProperty = CompartmentUpdate::Inflow;
          else if (propertyName == "Outflow")
            ds.UpdateProperty = CompartmentUpdate::Outflow;
        }
        ds.GasCmpt = gasCmpt;
        s = gasCmpt->GetScalar(propertyName);
      }
      break;
    }
    case eDataRequest_Category::LiquidCompartment:
    {
      if (!m_CmptMgr->HasLiquidCompartment(dr.GetCompartmentName()))
      {
        Error("Unknown liquid compartment : " + dr.GetCompartmentName());
        return false;
      }
      // Removing const because I need to create objects in order to track those objects
      SELiquidCompartment* liquidCmpt = (SELiquidCompartment*)m_CmptMgr->GetLiquidCompartment(dr.GetCompartmentName());

      if (dr.HasSubstanceName())
      {
        SESubstance* sub = m_SubMgr->GetSubstance(dr.GetSubstanceName());
        // Activate this substance so compartments have it
        m_SubMgr->AddActiveSubstance(*sub);
        if (liquidCmpt->HasChildren())
        {
          if (propertyName == "Mass")
            ds.UpdateProperty = CompartmentUpdate::Mass;
          else if (propertyName == "Concentration")
            ds.UpdateProperty = CompartmentUpdate::Concentration;
          else if (propertyName == "Molarity")
            ds.UpdateProperty = CompartmentUpdate::Molarity;
          else if (propertyName == "PartialPressure")
            ds.UpdateProperty = CompartmentUpdate::PartialPressure;
          else if (propertyName == "Saturation")
            ds.UpdateProperty = CompartmentUpdate::Saturation;
          ds.LiquidSubstance = liquidCmpt->GetSubstanceQuantity(*sub);
        }
        s = liquidCmpt->GetSubstanceQuantity(*sub)->GetScalar(propertyName);
      }
      else
      {
        if (liquidCmpt->HasChildren() || liquidCmpt->HasNodeMapping())
        {
          if (propertyName == "Volume")
            ds.UpdateProperty = CompartmentUpdate::Volume;
          if (propertyName == "Pressure")
            ds.UpdateProperty = CompartmentUpdate::Pressure;
        }

        {// Always Update these
          if (propertyName == "Inflow")
            ds.UpdateProperty = CompartmentUpdate::Inflow;
          else if (propertyName == "Outflow")
            ds.UpdateProperty = CompartmentUpdate::Outflow;
        }
        ds.LiquidCmpt = liquidCmpt;
        s = liquidCmpt->GetScalar(propertyName);
      }
      break;
    }
    case eDataRequest_Category::ThermalCompartment:
    {
      if (!m_CmptMgr->HasThermalCompartment(dr.GetCompartmentName()))
      {
        Error("Unknown thermal compartment : " + dr.GetCompartmentName());
        return false;
      }
      // Removing const because I need to create objects in order to track those objects
      SEThermalCompartment* thermalCmpt = (SEThermalCompartment*)m_CmptMgr->GetThermalCompartment(dr.GetCompartmentName());

      if (thermalCmpt->HasChildren() || thermalCmpt->HasNodeMapping())
      {
        if (propertyName == "Heat")
          ds.UpdateProperty = CompartmentUpdate::Heat;
        if (propertyName == "Temperature")
          ds.UpdateProperty = CompartmentUpdate::Temperature;
      }

      {// Always Update these       
        if (propertyName == "HeatTransferRateIn")
          ds.UpdateProperty = CompartmentUpdate::HeatTransferRateIn;
        else if (propertyName == "HeatTransferRateOut")
          ds.UpdateProperty = CompartmentUpdate::HeatTransferRateOut;
      }
      ds.ThermalCmpt = thermalCmpt;
      s = thermalCmpt->GetScalar(propertyName);
      break;
    }
    case eDataRequest_Category::TissueCompartment:
    {
      if (!m_CmptMgr->HasTissueCompartment(dr.GetCompartmentName()))
      {
        Error("Unknown tissue compartment : " + dr.GetCompartmentName());
        return false;
      }
      // Removing const because I need to create objects in order to track those objects
      SETissueCompartment* tissueCmpt = (SETissueCompartment*)m_CmptMgr->GetTissueCompartment(dr.GetCompartmentName());
      s = tissueCmpt->GetScalar(propertyName);
      break;
    }
    case eDataRequest_Category::Substance:
    {
      // Removing const because I want to allocate and grab scalars to track for later
      SESubstance* sub = m_SubMgr->GetSubstance(dr.GetSubstanceName());
      m_SubMgr->AddActiveSubstance(*sub);
      if (dr.HasCompartmentName())
      {// I don't really have a generic/reflexive way of doing this...yet
        if (dr.GetPropertyName() == "PartitionCoefficient")
        {
          SESubstanceTissuePharmacokinetics& tk = sub->GetPK().GetTissueKinetics(dr.GetCompartmentName());
          s = &tk.GetPartitionCoefficient();
          break;
        }
      }
      else
      {
        s = sub->GetScalar(propertyName);
        break;
      }
    }
    default:
    {
      m_ss << "Unhandled data request category: " << eDataRequest_Category_Name(dr.GetCategory()) << std::endl;
      Error(m_ss);
    }
  }

  if (s != nullptr)
  {
    ds.SetScalarRequest(*s, dr);
    return true;
  }
  m_ss << "Unhandled data request : " << propertyName << std::endl;
  Error(m_ss);
  return false;
}

void SEEngineTracker::TrackData(double time_s, double dt_s)
{
  if (!m_DataRequestMgr->HasDataRequests())
    return;// Nothing to do here...
  if (time_s == m_LastPullTime_s)
    return;

  double sampleTime_s = m_DataRequestMgr->GetSamplesPerSecond();
  if (sampleTime_s != 0)
    sampleTime_s = 1 / sampleTime_s;

  m_CurrentSampleTime_s += dt_s;
  if (m_CurrentSampleTime_s >= sampleTime_s)
  {
    m_CurrentSampleTime_s = 0;

    for (SEDataRequest* dr : m_DataRequestMgr->GetDataRequests())
    {
      SEDataRequestScalar* ds = m_Request2Scalar[dr];
      if (ds == nullptr)
      {
        Error("No SEDataRequestScalar for data request " + dr->GetPropertyName());
        continue;
      }
      if (!ds->HasScalar())
      {
        m_DataTrack->Probe(ds->idx, SEScalar::dNaN());
        continue;
      }
      ds->UpdateScalar();// Update compartment if needed
      if (ds->IsValid())
      {
        if (ds->HasUnit())
        {
          if (dr->GetUnit() == nullptr)
            dr->SetUnit(*ds->GetUnit());
          m_DataTrack->Probe(ds->idx, ds->GetValue(*dr->GetUnit()));
        }
        else
          m_DataTrack->Probe(ds->idx, ds->GetValue());
      }
      else if (ds->IsInfinity())
        m_DataTrack->Probe(ds->idx, std::numeric_limits<double>::infinity());
      else
        m_DataTrack->Probe(ds->idx, SEScalar::dNaN());
    }

    if (m_Mode == TrackMode::CSV)
      m_DataTrack->StreamProbesToFile(time_s, *m_ResultsStream);
  }

  m_LastPullTime_s = time_s;
}

const SEDataRequestScalar* SEEngineTracker::GetScalar(const SEDataRequest& dr) const
{
  auto found = m_Request2Scalar.find(&dr);
  if (found == m_Request2Scalar.end())
    return nullptr;
  return found->second;
}

void SEDataRequestScalar::SetScalarRequest(const SEScalar& s, SEDataRequest& dr)
{
  SEGenericScalar::SetScalar(s);
  if (m_UnitScalar != nullptr)
  {
    if (!dr.HasRequestedUnit())// Use set unit if none provide
    {
      if (!dr.HasUnit())//use the unit it has if there is no requested unit
      {
        if(HasUnit())
          dr.SetUnit(*GetUnit());
        else
        {
          Error("I have no idea what unit you want this data request in : " + dr.GetPropertyName());
        }
      }
    }
    else
    {
      const CCompoundUnit* unit = GetCompoundUnit(dr.GetRequestedUnit());
      if (unit==nullptr)
      {
        std::stringstream ss;
        ss << dr.GetRequestedUnit() << " is not compatible with " << dr.GetPropertyName();
        Fatal(ss);
      }
      else
        dr.SetUnit(*unit);
    }
  }
}

void SEDataRequestScalar::UpdateScalar()
{
  if (UpdateProperty == CompartmentUpdate::None)
    return;

  if (GasCmpt != nullptr)
  {
    switch (UpdateProperty)
    {
    case CompartmentUpdate::Inflow:
      GasCmpt->GetInflow();
      return;
    case CompartmentUpdate::Outflow:
      GasCmpt->GetOutflow();
      return;
    case CompartmentUpdate::Volume:
      GasCmpt->GetVolume();
      return;
    case CompartmentUpdate::Pressure:
      GasCmpt->GetPressure();
      return;
    default:    
      Error("Property is not supported on Gas Compartment");    
    }
  }
  else if (GasSubstance != nullptr)
  {
    switch (UpdateProperty)
    {
    case CompartmentUpdate::Volume:
      GasSubstance->GetVolume();
      return;
    case CompartmentUpdate::VolumeFraction:
      GasSubstance->GetVolumeFraction();
      return;
    case CompartmentUpdate::PartialPressure:
      GasSubstance->GetPartialPressure();
      return;
    default:
      Error("Property is not supported on Gas Substance");
    }
  }
  else if (LiquidCmpt != nullptr)
  {
    switch (UpdateProperty)
    {
    case CompartmentUpdate::Inflow:
      LiquidCmpt->GetInflow();
      return;
    case CompartmentUpdate::Outflow:
      LiquidCmpt->GetOutflow();
      return;
    case CompartmentUpdate::Volume:
      LiquidCmpt->GetVolume();
      return;
    case CompartmentUpdate::pH:
      LiquidCmpt->GetPH();
      return;
    case CompartmentUpdate::Pressure:
      LiquidCmpt->GetPressure();
      return;
    default:
      Error("Property is not supported on liquid Compartment");
    }
  }
  else if (LiquidSubstance != nullptr)
  {
    switch (UpdateProperty)
    {
    case CompartmentUpdate::Mass:
      LiquidSubstance->GetMass();
      return;
    case CompartmentUpdate::Molarity:
      LiquidSubstance->GetMolarity();
      return;
    case CompartmentUpdate::Concentration:
      LiquidSubstance->GetConcentration();
      return;
    case CompartmentUpdate::PartialPressure:
      LiquidSubstance->GetPartialPressure();
      return;
    case CompartmentUpdate::Saturation:
      LiquidSubstance->GetSaturation();
      return;
    default:
      Error("Property is not supported on Liquid Substance");
    }
  }
  else if (ThermalCmpt != nullptr)
  {
    switch (UpdateProperty)
    {
    case CompartmentUpdate::HeatTransferRateIn:
      ThermalCmpt->GetHeatTransferRateIn();
      return;
    case CompartmentUpdate::HeatTransferRateOut:
      ThermalCmpt->GetHeatTransferRateOut();
      return;   
    case CompartmentUpdate::Heat:
      ThermalCmpt->GetHeat();
      return;
    case CompartmentUpdate::Temperature:
      ThermalCmpt->GetTemperature();
      return;
    default:
      Error("Property is not supported on Thermal Substance");
    }
  }
  Error("Could not update " + Heading);
}
