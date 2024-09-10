/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "TCCCIteration.h"

#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/utils/GeneralMath.h"

namespace pulse::study::patient_variability
{
  TCCCIteration::TCCCIteration(Logger& logger) : ActionIteration(logger)
  {
    m_IterationName = "TCCC";
    SetStateDirectory("");
    SetScenarioExecListFilename("");

    m_PerformInterventions = false;

    // Default (currently) non-exposed configurations
    m_BrainInjury.SetType(eBrainInjury_Type::LeftFocal);
    m_LeftHemothorax.SetSide(eSide::Left);
    m_RightHemothorax.SetSide(eSide::Right);
    m_LeftTensionPneumothorax.SetSide(eSide::Left);
    m_RightTensionPneumothorax.SetSide(eSide::Right);

    m_DataRequestMgr->CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
    m_DataRequestMgr->CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
    m_DataRequestMgr->CreatePhysiologyDataRequest("OxygenSaturation");
    m_DataRequestMgr->CreatePhysiologyDataRequest("PeripheralPerfusionIndex");
    m_DataRequestMgr->CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
    m_DataRequestMgr->CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
    m_DataRequestMgr->CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
    m_DataRequestMgr->CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
    m_DataRequestMgr->CreatePhysiologyDataRequest("CoreTemperature", TemperatureUnit::C);
    m_DataRequestMgr->CreatePhysiologyDataRequest("SkinTemperature", TemperatureUnit::C);

    m_DataRequestMgr->CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
    m_DataRequestMgr->CreatePhysiologyDataRequest("TotalHemorrhageRate", VolumePerTimeUnit::mL_Per_min);
    m_DataRequestMgr->CreatePhysiologyDataRequest("TotalHemorrhagedVolume", VolumeUnit::mL);
    m_DataRequestMgr->CreateLiquidCompartmentDataRequest("BrainVasculature", "Oxygen", "PartialPressure", PressureUnit::mmHg);

    m_DataRequestMgr->CreatePatientDataRequest("VitalCapacity", VolumeUnit::mL);
    m_DataRequestMgr->CreatePatientDataRequest("ResidualVolume", VolumeUnit::mL);
    m_DataRequestMgr->CreatePhysiologyDataRequest("EndTidalCarbonDioxideFraction");
    m_DataRequestMgr->CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
    m_DataRequestMgr->CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
    m_DataRequestMgr->CreatePhysiologyDataRequest("HorowitzIndex", PressureUnit::cmH2O);
    m_DataRequestMgr->CreateGasCompartmentDataRequest("Carina", "Oxygen", "PartialPressure", PressureUnit::mmHg);


  }
  TCCCIteration::~TCCCIteration()
  {

  }

  void TCCCIteration::Clear()
  {
    m_Actions.clear();
    ScenarioIteration::Clear();
  }

  void TCCCIteration::FixUp()
  {
    if (m_AirwayObstructionSeverity.Empty())
      m_AirwayObstructionSeverity.SetValues({ 0 });
    if (m_HemorrhageSeverity.Empty())
      m_HemorrhageSeverity.SetValues({ 0 });
    if (m_HemorrhageWound.Empty())
      m_HemorrhageWound.GetValues().push_back(0);
    if (m_LeftHemothoraxSeverity.Empty())
      m_LeftHemothoraxSeverity.SetValues({ 0 });
    if (m_RightHemothoraxSeverity.Empty())
      m_RightHemothoraxSeverity.SetValues({ 0 });
    if (m_StressSeverity.Empty())
      m_StressSeverity.SetValues({ 0 });
    if (m_TBISeverity.Empty())
      m_TBISeverity.SetValues({ 0 });
    if (m_LeftTensionPneumothoraxSeverity.Empty())
      m_LeftTensionPneumothoraxSeverity.SetValues({ 0 });
    if (m_LeftTensionPneumothoraxWound.Empty())
      m_LeftTensionPneumothoraxWound.GetValues().push_back(0);
    if (m_RightTensionPneumothoraxSeverity.Empty())
      m_RightTensionPneumothoraxSeverity.SetValues({ 0 });
    if (m_RightTensionPneumothoraxWound.Empty())
      m_RightTensionPneumothoraxWound.GetValues().push_back(0);

    if (m_InsultDuration_s.Empty())
      m_InsultDuration_s.SetValues({ 0 });
    if (m_SalineAvailable.Empty())
      m_SalineAvailable.SetValues({ 0 });
    if (m_NeedleAvailable.Empty())
      m_NeedleAvailable.SetValues({ 0 });
    if (m_ChestWrapAvailable.Empty())
      m_ChestWrapAvailable.SetValues({ 0 });
  }

  void TCCCIteration::GenerateSlicedActionSets(std::pair<std::string, std::string> patientFolderAndStateFilename)
  {
    double AirwayObstructionSeverity = 0;
    double HemorrhageSeverity = 0;
    size_t HemorrhageWound = 0;
    double LeftHemothoraxSeverity = 0;
    double RightHemothoraxSeverity = 0;
    double StressSeverity = 0;
    double LeftTensionPneumothoraxSeverity = 0;
    size_t LeftTensionPneumothoraxWound = 0;
    double RightTensionPneumothoraxSeverity = 0;
    size_t RightTensionPneumothoraxWound = 0;
    double TBISeverity = 0;
    double InsultDuration_s = 0;

    SetDescription(patientFolderAndStateFilename.first);
    SetEngineStateFile(patientFolderAndStateFilename.second);

    for (double s : m_AirwayObstructionSeverity.GetValues())
    {
      if (s == 0) continue;
      GenerateScenario(s, HemorrhageSeverity, HemorrhageWound,
        LeftHemothoraxSeverity, RightHemothoraxSeverity, StressSeverity, TBISeverity,
        LeftTensionPneumothoraxSeverity, LeftTensionPneumothoraxWound,
        RightTensionPneumothoraxSeverity, RightTensionPneumothoraxWound,
        InsultDuration_s, patientFolderAndStateFilename.first);
    }

    for (double s : m_HemorrhageSeverity.GetValues())
    {
      if (s == 0) continue;
      for (size_t w : m_HemorrhageWound.GetValues())
      {
        GenerateScenario(AirwayObstructionSeverity, s, w,
          LeftHemothoraxSeverity, RightHemothoraxSeverity, StressSeverity, TBISeverity,
          LeftTensionPneumothoraxSeverity, LeftTensionPneumothoraxWound,
          RightTensionPneumothoraxSeverity, RightTensionPneumothoraxWound,
          InsultDuration_s, patientFolderAndStateFilename.first);
      }
    }

    for (double s : m_LeftHemothoraxSeverity.GetValues())
    {
      if (s == 0) continue;
      GenerateScenario(AirwayObstructionSeverity, HemorrhageSeverity, HemorrhageWound,
        s, RightHemothoraxSeverity, StressSeverity, TBISeverity,
        LeftTensionPneumothoraxSeverity, LeftTensionPneumothoraxWound,
        RightTensionPneumothoraxSeverity, RightTensionPneumothoraxWound,
        InsultDuration_s, patientFolderAndStateFilename.first);
    }

    for (double s : m_RightHemothoraxSeverity.GetValues())
    {
      if (s == 0) continue;
      GenerateScenario(AirwayObstructionSeverity, HemorrhageSeverity, HemorrhageWound,
        LeftHemothoraxSeverity, s, StressSeverity, TBISeverity,
        LeftTensionPneumothoraxSeverity, LeftTensionPneumothoraxWound,
        RightTensionPneumothoraxSeverity, RightTensionPneumothoraxWound,
        InsultDuration_s, patientFolderAndStateFilename.first);
    }

    for (double s : m_StressSeverity.GetValues())
    {
      if (s == 0) continue;
      GenerateScenario(AirwayObstructionSeverity, HemorrhageSeverity, HemorrhageWound,
        LeftHemothoraxSeverity, RightHemothoraxSeverity, s, TBISeverity,
        LeftTensionPneumothoraxSeverity, LeftTensionPneumothoraxWound,
        RightTensionPneumothoraxSeverity, RightTensionPneumothoraxWound,
        InsultDuration_s, patientFolderAndStateFilename.first);
    }

    for (double s : m_TBISeverity.GetValues())
    {
      if (s == 0) continue;
      GenerateScenario(AirwayObstructionSeverity, HemorrhageSeverity, HemorrhageWound,
        LeftHemothoraxSeverity, RightHemothoraxSeverity, StressSeverity, TBISeverity,
        LeftTensionPneumothoraxSeverity, LeftTensionPneumothoraxWound,
        RightTensionPneumothoraxSeverity, RightTensionPneumothoraxWound,
        InsultDuration_s, patientFolderAndStateFilename.first);
    }

    for (double s : m_LeftTensionPneumothoraxSeverity.GetValues())
    {
      if (s == 0) continue;
      for (size_t w : m_LeftTensionPneumothoraxWound.GetValues())
      {
        GenerateScenario(AirwayObstructionSeverity, s, w,
          LeftHemothoraxSeverity, RightHemothoraxSeverity, StressSeverity, TBISeverity,
          s, w,
          RightTensionPneumothoraxSeverity, RightTensionPneumothoraxWound,
          InsultDuration_s, patientFolderAndStateFilename.first);
      }
    }

    for (double s : m_RightTensionPneumothoraxSeverity.GetValues())
    {
      if (s == 0) continue;
      for (size_t w : m_RightTensionPneumothoraxWound.GetValues())
      {
        GenerateScenario(AirwayObstructionSeverity, s, w,
          LeftHemothoraxSeverity, RightHemothoraxSeverity, StressSeverity, TBISeverity,
          LeftTensionPneumothoraxSeverity, LeftTensionPneumothoraxWound,
          s, w,
          InsultDuration_s, patientFolderAndStateFilename.first);
      }
    }

    m_Actions.clear();
  }

  void TCCCIteration::GenerateCombinationActionSets(std::pair<std::string, std::string> patientFolderAndStateFilename)
  {
    std::vector<size_t> opts; // -1 as the opts holds the max index of that option
    opts.push_back(m_AirwayObstructionSeverity.GetValues().size()-1);
    opts.push_back(m_HemorrhageSeverity.GetValues().size()-1);
    opts.push_back(m_HemorrhageWound.GetValues().size()-1);
    opts.push_back(m_LeftHemothoraxSeverity.GetValues().size() - 1);
    opts.push_back(m_RightHemothoraxSeverity.GetValues().size() - 1);
    opts.push_back(m_StressSeverity.GetValues().size() - 1);
    opts.push_back(m_TBISeverity.GetValues().size() - 1);
    opts.push_back(m_LeftTensionPneumothoraxSeverity.GetValues().size()-1);
    opts.push_back(m_LeftTensionPneumothoraxWound.GetValues().size() - 1);
    opts.push_back(m_RightTensionPneumothoraxSeverity.GetValues().size() - 1);
    opts.push_back(m_RightTensionPneumothoraxWound.GetValues().size() - 1);
    opts.push_back(m_InsultDuration_s.GetValues().size()-1);
    opts.push_back(m_SalineAvailable.GetValues().size()-1);
    opts.push_back(m_NeedleAvailable.GetValues().size()-1);
    opts.push_back(m_ChestWrapAvailable.GetValues().size()-1);

    std::vector<std::vector<size_t>> permutations;
    GeneralMath::Combinations(opts, permutations);

    double AirwayObstructionSeverity = 0;
    double HemorrhageSeverity = 0;
    size_t HemorrhageWound = 0;
    double LeftHemothoraxSeverity = 0;
    double RightHemothoraxSeverity = 0;
    double StressSeverity = 0;
    double LeftTensionPneumothoraxSeverity = 0;
    size_t LeftTensionPneumothoraxWound = 0;
    double RightTensionPneumothoraxSeverity = 0;
    size_t RightTensionPneumothoraxWound = 0;
    double TBISeverity = 0;
    double InsultDuration_s = 0;

    SetDescription(patientFolderAndStateFilename.first);
    SetEngineStateFile(patientFolderAndStateFilename.second);

    for (auto idxs : permutations)
    {
      AirwayObstructionSeverity = m_AirwayObstructionSeverity.GetValues()[idxs[0]];
      HemorrhageSeverity = m_HemorrhageSeverity.GetValues()[idxs[1]];
      HemorrhageWound = m_HemorrhageWound.GetValues()[idxs[2]];
      LeftHemothoraxSeverity = m_LeftHemothoraxSeverity.GetValues()[idxs[3]];
      RightHemothoraxSeverity = m_RightHemothoraxSeverity.GetValues()[idxs[4]];
      StressSeverity = m_StressSeverity.GetValues()[idxs[5]];
      TBISeverity = m_TBISeverity.GetValues()[idxs[6]];
      LeftTensionPneumothoraxSeverity = m_LeftTensionPneumothoraxSeverity.GetValues()[idxs[7]];
      LeftTensionPneumothoraxWound = m_LeftTensionPneumothoraxWound.GetValues()[idxs[8]];
      RightTensionPneumothoraxSeverity = m_RightTensionPneumothoraxSeverity.GetValues()[idxs[9]];
      RightTensionPneumothoraxWound = m_RightTensionPneumothoraxWound.GetValues()[idxs[10]];
      InsultDuration_s = m_InsultDuration_s.GetValues()[idxs[11]];
      if (AirwayObstructionSeverity > 0 ||
          HemorrhageSeverity > 0 ||
          LeftHemothoraxSeverity > 0 ||
          RightHemothoraxSeverity > 0 ||
          StressSeverity > 0 ||
          TBISeverity > 0 ||
          LeftTensionPneumothoraxSeverity > 0 ||
          RightTensionPneumothoraxSeverity > 0)
      {
        GenerateScenario(AirwayObstructionSeverity, HemorrhageSeverity, HemorrhageWound,
          LeftHemothoraxSeverity, RightHemothoraxSeverity, StressSeverity, TBISeverity,
          LeftTensionPneumothoraxSeverity, LeftTensionPneumothoraxWound,
          RightTensionPneumothoraxSeverity, RightTensionPneumothoraxWound,
          InsultDuration_s, patientFolderAndStateFilename.first);
      }
      m_Actions.clear();
    }
  }

  void TCCCIteration::GenerateScenario(double AirwayObstructionSeverity,
                                       double HemorrhageSeverity,
                                       size_t HemorrhageWound,
                                       double LeftHemothoraxSeverity,
                                       double RightHemothoraxSeverity,
                                       double StressSeverity,
                                       double TBISeverity,
                                       double LeftTensionPneumothoraxSeverity,
                                       size_t LeftTensionPneumothoraxWound,
                                       double RightTensionPneumothoraxSeverity,
                                       size_t RightTensionPneumothoraxWound,
                                       double InsultDuration_s,
                                       const std::string& PatientName)
  {
    // Names are getting way long...
    // Let's just create a name with only the injuries
    std::string name;
    if (AirwayObstructionSeverity>0)
      name =  "AO" + pulse::cdm::to_string(AirwayObstructionSeverity) + "_";
    if (HemorrhageSeverity > 0)
    {
      name += "HEM" + pulse::cdm::to_string(HemorrhageSeverity);
      switch ((eHemorrhageWound)HemorrhageWound) {
      case eHemorrhageWound::LeftLegLaceration:
        name += "LL_";
        break;
      case eHemorrhageWound::RightArmLaceration:
        name += "RA_";
        break;
      case eHemorrhageWound::RightArmRightLegLaceration:
        name += "RARL_";
        break;
      case eHemorrhageWound::RightLegLeftLegLaceration:
        name += "RLLL_";
        break;
      case eHemorrhageWound::InternalLiver:
        name += "InL_";
        break;
      case eHemorrhageWound::ExternalLiver:
        name += "ExL_";
        break;
      case eHemorrhageWound::_COUNT:
      default:
        name += "Undefined_";
        break;
      }
    }
    if (LeftHemothoraxSeverity > 0)
      name += "LHTX" + pulse::cdm::to_string(LeftHemothoraxSeverity) + "_";
    if (RightHemothoraxSeverity > 0)
      name += "RHTX" + pulse::cdm::to_string(RightHemothoraxSeverity) + "_";
    if (StressSeverity > 0)
      name += "S" + pulse::cdm::to_string(StressSeverity) + "_";
    if (TBISeverity > 0)
      name += "TBI" + pulse::cdm::to_string(TBISeverity) + "_";
    if (LeftTensionPneumothoraxSeverity > 0)
    {
      name += "LTP" + pulse::cdm::to_string(LeftTensionPneumothoraxSeverity);
      switch ((eTensionPneumothoraxWound)LeftTensionPneumothoraxWound) {
      case eTensionPneumothoraxWound::Closed:
        name += "C_";
        break;
      case eTensionPneumothoraxWound::Open:
        name += "O_";
        break;
      case eTensionPneumothoraxWound::_COUNT:
      default:
        name += "Undefined_";
        break;
      }
    }
    if (RightTensionPneumothoraxSeverity > 0)
    {
      name += "RTP" + pulse::cdm::to_string(RightTensionPneumothoraxSeverity);
      switch ((eTensionPneumothoraxWound)RightTensionPneumothoraxWound) {
      case eTensionPneumothoraxWound::Closed:
        name += "C_";
        break;
      case eTensionPneumothoraxWound::Open:
        name += "O_";
        break;
      case eTensionPneumothoraxWound::_COUNT:
      default:
        name += "Undefined_";
        break;
      }
    }
    name += "D"+pulse::cdm::to_string(InsultDuration_s)+"s";
    SetName(PatientName +"/"+name);
    if (m_ScenarioStates.find(m_Name) != m_ScenarioStates.end())
    {
      Info("Ignoring duplicate TCCC scenario: " + m_Name);
      m_Duplicates++;
      return;
    }
    GetDataRequestManager().SetResultsFilename(m_ResultsDirectory+m_Name + ".csv");

    double TotalAdvanceTime_s = 0;
    double FinalAdvanceTime_min = 0;

    // Advance to the insults
    if (m_BaselineDuration_s > 0)
    {
      m_Adv2Insult.GetTime().SetValue(m_BaselineDuration_s, TimeUnit::s);
      m_Actions.push_back(&m_Adv2Insult);
      TotalAdvanceTime_s = m_BaselineDuration_s;
    }

    // Apply the insults
    if (AirwayObstructionSeverity > 0)
    {
      m_AirwayObstruction.GetSeverity().SetValue(AirwayObstructionSeverity);
      m_Actions.push_back(&m_AirwayObstruction);
    }
    if (HemorrhageSeverity > 0)
    {
      switch ((eHemorrhageWound)HemorrhageWound) {
        case eHemorrhageWound::LeftLegLaceration:
          m_Hemorrhage1.SetType(eHemorrhage_Type::External);
          m_Hemorrhage1.SetCompartment(eHemorrhage_Compartment::RightLeg);
          m_Hemorrhage1.GetSeverity().SetValue(HemorrhageSeverity);
          m_Actions.push_back(&m_Hemorrhage1);
          break;
        case eHemorrhageWound::RightArmLaceration:
          m_Hemorrhage1.SetType(eHemorrhage_Type::External);
          m_Hemorrhage1.SetCompartment(eHemorrhage_Compartment::RightArm);
          m_Hemorrhage1.GetSeverity().SetValue(HemorrhageSeverity);
          m_Actions.push_back(&m_Hemorrhage1);
          break;
        case eHemorrhageWound::RightArmRightLegLaceration:
          m_Hemorrhage1.SetType(eHemorrhage_Type::External);
          m_Hemorrhage1.SetCompartment(eHemorrhage_Compartment::RightArm);
          m_Hemorrhage1.GetSeverity().SetValue(HemorrhageSeverity);
          m_Actions.push_back(&m_Hemorrhage1);
          m_Hemorrhage2.SetType(eHemorrhage_Type::External);
          m_Hemorrhage2.SetCompartment(eHemorrhage_Compartment::RightLeg);
          m_Hemorrhage2.GetSeverity().SetValue(HemorrhageSeverity);
          m_Actions.push_back(&m_Hemorrhage2);
          break;
        case eHemorrhageWound::RightLegLeftLegLaceration:
          m_Hemorrhage1.SetType(eHemorrhage_Type::External);
          m_Hemorrhage1.SetCompartment(eHemorrhage_Compartment::RightLeg);
          m_Hemorrhage1.GetSeverity().SetValue(HemorrhageSeverity);
          m_Actions.push_back(&m_Hemorrhage1);
          m_Hemorrhage2.SetType(eHemorrhage_Type::External);
          m_Hemorrhage2.SetCompartment(eHemorrhage_Compartment::LeftLeg);
          m_Hemorrhage2.GetSeverity().SetValue(HemorrhageSeverity);
          m_Actions.push_back(&m_Hemorrhage2);
          break;
        case eHemorrhageWound::InternalLiver:
          m_Hemorrhage1.SetType(eHemorrhage_Type::Internal);
          m_Hemorrhage1.SetCompartment(eHemorrhage_Compartment::Liver);
          m_Hemorrhage1.GetSeverity().SetValue(HemorrhageSeverity);
          m_Actions.push_back(&m_Hemorrhage1);
          break;
        case eHemorrhageWound::ExternalLiver:
          m_Hemorrhage1.SetType(eHemorrhage_Type::External);
          m_Hemorrhage1.SetCompartment(eHemorrhage_Compartment::Liver);
          m_Hemorrhage1.GetSeverity().SetValue(HemorrhageSeverity);
          m_Actions.push_back(&m_Hemorrhage1);
          break;
        case eHemorrhageWound::_COUNT:
        default:
          break;
      }
    }
    if (LeftHemothoraxSeverity > 0)
    {
      m_LeftHemothorax.GetSeverity().SetValue(LeftHemothoraxSeverity);
      m_Actions.push_back(&m_LeftHemothorax);
    }
    if (RightHemothoraxSeverity > 0)
    {
      m_RightHemothorax.GetSeverity().SetValue(RightHemothoraxSeverity);
      m_Actions.push_back(&m_RightHemothorax);
    }
    if (StressSeverity > 0)
    {
      m_Stress.GetSeverity().SetValue(StressSeverity);
      m_Actions.push_back(&m_Stress);
    }
    if (TBISeverity > 0)
    {
      m_BrainInjury.GetSeverity().SetValue(TBISeverity);
      m_Actions.push_back(&m_BrainInjury);
    }
    if (LeftTensionPneumothoraxSeverity > 0)
    {
      switch ((eTensionPneumothoraxWound)LeftTensionPneumothoraxWound) {
        case eTensionPneumothoraxWound::Closed:
          m_LeftTensionPneumothorax.SetType(eGate::Closed);
          m_LeftTensionPneumothorax.GetSeverity().SetValue(LeftTensionPneumothoraxSeverity);
          m_Actions.push_back(&m_LeftTensionPneumothorax);
          break;
        case eTensionPneumothoraxWound::Open:
          m_LeftTensionPneumothorax.SetType(eGate::Open);
          m_LeftTensionPneumothorax.GetSeverity().SetValue(LeftTensionPneumothoraxSeverity);
          m_Actions.push_back(&m_LeftTensionPneumothorax);
          break;
        case eTensionPneumothoraxWound::_COUNT:
        default:
          break;
      }
    }
    if (RightTensionPneumothoraxSeverity > 0)
    {
      switch ((eTensionPneumothoraxWound)RightTensionPneumothoraxWound) {
      case eTensionPneumothoraxWound::Closed:
        m_RightTensionPneumothorax.SetType(eGate::Closed);
        m_RightTensionPneumothorax.GetSeverity().SetValue(RightTensionPneumothoraxSeverity);
        m_Actions.push_back(&m_RightTensionPneumothorax);
        break;
      case eTensionPneumothoraxWound::Open:
        m_RightTensionPneumothorax.SetType(eGate::Open);
        m_RightTensionPneumothorax.GetSeverity().SetValue(RightTensionPneumothoraxSeverity);
        m_Actions.push_back(&m_RightTensionPneumothorax);
        break;
      case eTensionPneumothoraxWound::_COUNT:
      default:
        break;
      }
    }

    // Advance the duration of the insult
    if (InsultDuration_s > 0)
    {
      TotalAdvanceTime_s += InsultDuration_s;
      m_Adv2Intervention.GetTime().SetValue(InsultDuration_s, TimeUnit::s);
      m_Actions.push_back(&m_Adv2Intervention);
    }

    // Save State
    m_Serialize.SetFilename(m_StateDirectory+"/"+m_Name+".pbb");
    m_Actions.push_back(&m_Serialize);

    if (m_PerformInterventions)
    {
      // TODO
    }

    // Advance the duration of the scenario
    FinalAdvanceTime_min = m_MaxSimTime_min - (TotalAdvanceTime_s / 60);
    if (FinalAdvanceTime_min > 0)
    {
      m_Adv2End.GetTime().SetValue(FinalAdvanceTime_min, TimeUnit::min);
      m_Actions.push_back(&m_Adv2End);
    }

    // Track and Write the scenario
    WriteScenario();
  }
}
