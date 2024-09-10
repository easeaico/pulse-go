/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
#include "cdm/patient/actions/SEAcuteStress.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEArrhythmia.h"
#include "cdm/patient/actions/SEAsthmaAttack.h"
#include "cdm/patient/actions/SEBrainInjury.h"
#include "cdm/patient/actions/SEBronchoconstriction.h"
#include "cdm/patient/actions/SECardiovascularMechanicsModification.h"
#include "cdm/patient/actions/SEChestCompression.h"
#include "cdm/patient/actions/SEChestCompressionAutomated.h"
#include "cdm/patient/actions/SEChestCompressionInstantaneous.h"
#include "cdm/patient/actions/SEChestOcclusiveDressing.h"
#include "cdm/patient/actions/SEConsciousRespiration.h"
#include "cdm/patient/actions/SEConsumeNutrients.h"
#include "cdm/patient/actions/SEChronicObstructivePulmonaryDiseaseExacerbation.h"
#include "cdm/patient/actions/SEExercise.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SEHemothorax.h"
#include "cdm/patient/actions/SEImpairedAlveolarExchangeExacerbation.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/patient/actions/SEMechanicalVentilation.h"
#include "cdm/patient/actions/SENeedleDecompression.h"
#include "cdm/patient/actions/SEPericardialEffusion.h"
#include "cdm/patient/actions/SEPneumoniaExacerbation.h"
#include "cdm/patient/actions/SEPulmonaryShuntExacerbation.h"
#include "cdm/patient/actions/SERespiratoryFatigue.h"
#include "cdm/patient/actions/SERespiratoryMechanicsConfiguration.h"
#include "cdm/patient/actions/SERespiratoryMechanicsModification.h"
#include "cdm/patient/actions/SESubstanceBolus.h"
#include "cdm/patient/actions/SESubstanceCompoundInfusion.h"
#include "cdm/patient/actions/SESubstanceInfusion.h"
#include "cdm/patient/actions/SESupplementalOxygen.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/patient/actions/SETubeThoracostomy.h"
#include "cdm/patient/actions/SEUrinate.h"
#include "cdm/patient/actions/SEPatientAssessmentRequest.h"

#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceCompound.h"
#include "cdm/substance/SESubstanceConcentration.h"

#include "cdm/properties/SEScalarVolumePerTime.h"

SEPatientActionCollection::SEPatientActionCollection(SESubstanceManager& subMgr) : Loggable(subMgr.GetLogger()), m_SubMgr(subMgr)
{
  m_ARDSExacerbation = nullptr;
  m_AcuteStress = nullptr;
  m_AirwayObstruction = nullptr;
  m_Arrhythmia = nullptr;
  m_AsthmaAttack = nullptr;
  m_BrainInjury = nullptr;
  m_Bronchoconstriction = nullptr;
  m_CardiovascularMechanicsModification = nullptr;
  m_ChestCompression = nullptr;
  m_ChestCompressionAutomated = nullptr;
  m_ChestCompressionInstantaneous = nullptr;
  m_ConsciousRespiration = nullptr;
  m_ConsumeNutrients = nullptr;
  m_COPDExacerbation = nullptr;
  m_LeftChestOcclusiveDressing = nullptr;
  m_RightChestOcclusiveDressing = nullptr;
  m_Dyspnea = nullptr;
  m_Exercise = nullptr;
  m_LeftHemothorax = nullptr;
  m_RightHemothorax = nullptr;
  m_ImpairedAlveolarExchangeExacerbation = nullptr;
  m_Intubation = nullptr;
  m_MechanicalVentilation = nullptr;
  m_LeftNeedleDecompression = nullptr;
  m_RightNeedleDecompression = nullptr;
  m_RespiratoryFatigue = nullptr;
  m_RespiratoryMechanicsConfiguration = nullptr;
  m_RespiratoryMechanicsModification = nullptr;
  m_PericardialEffusion = nullptr;
  m_PneumoniaExacerbation = nullptr;
  m_PulmonaryShuntExacerbation = nullptr;
  m_SupplementalOxygen = nullptr;
  m_LeftOpenTensionPneumothorax = nullptr;
  m_LeftClosedTensionPneumothorax = nullptr;
  m_RightOpenTensionPneumothorax = nullptr;
  m_RightClosedTensionPneumothorax = nullptr;
  m_LeftTubeThoracostomy = nullptr;
  m_RightTubeThoracostomy = nullptr;
  m_Urinate = nullptr;
}

SEPatientActionCollection::~SEPatientActionCollection()
{
  SAFE_DELETE(m_ARDSExacerbation);
  SAFE_DELETE(m_AcuteStress);
  SAFE_DELETE(m_AirwayObstruction);
  SAFE_DELETE(m_Arrhythmia);
  SAFE_DELETE(m_AsthmaAttack);
  SAFE_DELETE(m_BrainInjury);
  SAFE_DELETE(m_Bronchoconstriction);
  SAFE_DELETE(m_CardiovascularMechanicsModification);
  SAFE_DELETE(m_ChestCompression);
  SAFE_DELETE(m_ChestCompressionAutomated);
  SAFE_DELETE(m_ChestCompressionInstantaneous);
  SAFE_DELETE(m_LeftChestOcclusiveDressing);
  SAFE_DELETE(m_RightChestOcclusiveDressing);
  SAFE_DELETE(m_ConsciousRespiration);
  SAFE_DELETE(m_ConsumeNutrients);
  SAFE_DELETE(m_COPDExacerbation);
  SAFE_DELETE(m_Dyspnea);
  SAFE_DELETE(m_Exercise);
  SAFE_DELETE(m_LeftHemothorax);
  SAFE_DELETE(m_RightHemothorax);
  SAFE_DELETE(m_Intubation);
  SAFE_DELETE(m_ImpairedAlveolarExchangeExacerbation);
  SAFE_DELETE(m_MechanicalVentilation);
  SAFE_DELETE(m_LeftNeedleDecompression);
  SAFE_DELETE(m_RightNeedleDecompression);
  SAFE_DELETE(m_PericardialEffusion);
  SAFE_DELETE(m_PneumoniaExacerbation);
  SAFE_DELETE(m_PulmonaryShuntExacerbation);
  SAFE_DELETE(m_RespiratoryFatigue);
  SAFE_DELETE(m_RespiratoryMechanicsConfiguration);
  SAFE_DELETE(m_RespiratoryMechanicsModification);
  SAFE_DELETE(m_SupplementalOxygen);
  SAFE_DELETE(m_LeftClosedTensionPneumothorax);
  SAFE_DELETE(m_LeftOpenTensionPneumothorax);
  SAFE_DELETE(m_RightClosedTensionPneumothorax);
  SAFE_DELETE(m_RightOpenTensionPneumothorax);
  SAFE_DELETE(m_LeftTubeThoracostomy);
  SAFE_DELETE(m_RightTubeThoracostomy);
  SAFE_DELETE(m_Urinate);

  DELETE_VECTOR(m_Hemorrhages);
  DELETE_VECTOR(m_SubstanceBoluses);
  DELETE_VECTOR(m_SubstanceInfusions);
  DELETE_VECTOR(m_SubstanceCompoundInfusions);
}

void SEPatientActionCollection::Clear()
{
  RemoveAcuteRespiratoryDistressSyndromeExacerbation();
  RemoveAcuteStress();
  RemoveAirwayObstruction();
  RemoveArrhythmia();
  RemoveAsthmaAttack();
  RemoveBrainInjury();
  RemoveBronchoconstriction();
  RemoveCardiovascularMechanicsModification();
  RemoveChestCompressionAutomated();
  RemoveChestCompressionInstantaneous();
  RemoveChronicObstructivePulmonaryDiseaseExacerbation();
  RemoveConsciousRespiration();
  RemoveConsumeNutrients();
  RemoveLeftChestOcclusiveDressing();
  RemoveRightChestOcclusiveDressing();
  RemoveDyspnea();
  RemoveExercise();
  RemoveLeftHemothorax();
  RemoveRightHemothorax();
  RemoveImpairedAlveolarExchangeExacerbation();
  RemoveIntubation();
  RemoveMechanicalVentilation();
  RemoveLeftNeedleDecompression();
  RemoveRightNeedleDecompression();
  RemovePericardialEffusion();
  RemovePneumoniaExacerbation();
  RemovePulmonaryShuntExacerbation();
  RemoveRespiratoryFatigue();
  RemoveRespiratoryMechanicsConfiguration();
  RemoveRespiratoryMechanicsModification();
  RemoveLeftOpenTensionPneumothorax();
  RemoveLeftClosedTensionPneumothorax();
  RemoveRightOpenTensionPneumothorax();
  RemoveRightClosedTensionPneumothorax();
  RemoveLeftTubeThoracostomy();
  RemoveRightTubeThoracostomy();
  RemoveUrinate();

  for (auto a : m_Hemorrhages)
    a->Deactivate();
  for (auto a : m_SubstanceBoluses)
    a->Deactivate();
  for (auto a : m_SubstanceInfusions)
    a->Deactivate();
  for (auto a : m_SubstanceCompoundInfusions)
    a->Deactivate();
}

bool SEPatientActionCollection::ProcessAction(const SEPatientAction& action)
{
  const SEPatientAssessmentRequest* patientAss = dynamic_cast<const SEPatientAssessmentRequest*>(&action);
  if (patientAss != nullptr)
  {
    // TODO just add this to a list?
    // Not doing anything with this, assessment actions
    // are currently only being handled by the Engine ScenarioExec methods.
    // Which probably just write out the assessment requested to a file
    // Assessments are really a real time API thing, not really a scenario thing
    // If they are a scenario thing, its just to write out the data.
    return true;//?
  }

  // We are creating our own copy of these actions
  // There can be multiple actions in a Scenario that are associated
  // with a specific function, such as hemorrhage, and we only need
  // to have a single action in our list associated with a hemorrhage.
  // We just overwrite our saved action with the current state of that action
  // So if the 4 hemorrhage actions come in, and lower the rate each time for a
  // certain compartment, we just update the single hemorrhage action associated
  // with that compartment.
  // SO, we make our own copy and manage that copy (i.e. by updating a single action)

  const SEAcuteRespiratoryDistressSyndromeExacerbation* ards = dynamic_cast<const SEAcuteRespiratoryDistressSyndromeExacerbation*>(&action);
  if (ards != nullptr)
  {
    GetAcuteRespiratoryDistressSyndromeExacerbation().Copy(*ards, true);
    m_ARDSExacerbation->Activate();
    if (!m_ARDSExacerbation->IsActive())
      RemoveAcuteRespiratoryDistressSyndromeExacerbation();
    return true;
  }

  const SEAcuteStress* aStress = dynamic_cast<const SEAcuteStress*>(&action);
  if (aStress != nullptr)
  {
    GetAcuteStress().Copy(*aStress, true);
    m_AcuteStress->Activate();
    if (!m_AcuteStress->IsActive())
      RemoveAcuteStress();
    return true;
  }

  const SEAirwayObstruction* airwayObst = dynamic_cast<const SEAirwayObstruction*>(&action);
  if (airwayObst != nullptr)
  {
    GetAirwayObstruction().Copy(*airwayObst, true);
    m_AirwayObstruction->Activate();
    if (!m_AirwayObstruction->IsActive())
      RemoveAirwayObstruction();
    return true;
  }

  const SEArrhythmia* Arrhythmia = dynamic_cast<const SEArrhythmia*>(&action);
  if (Arrhythmia != nullptr)
  {
    GetArrhythmia().Copy(*Arrhythmia, true);
    m_Arrhythmia->Activate();
    if (!m_Arrhythmia->IsActive())
      RemoveArrhythmia();
    return true;
  }

  const SEAsthmaAttack* asthmaattack = dynamic_cast<const SEAsthmaAttack*>(&action);
  if (asthmaattack != nullptr)
  {
    GetAsthmaAttack().Copy(*asthmaattack, true);
    m_AsthmaAttack->Activate();
    if (!m_AsthmaAttack->IsActive())
      RemoveAsthmaAttack();
    return true;
  }

  const SEBrainInjury* brainInjury = dynamic_cast<const SEBrainInjury*>(&action);
  if (brainInjury != nullptr)
  {
    GetBrainInjury().Copy(*brainInjury, true);
    m_BrainInjury->Activate();
    if (!m_BrainInjury->IsActive())
      RemoveBrainInjury();
    return true;
  }

  const SEBronchoconstriction* bronchoconstr = dynamic_cast<const SEBronchoconstriction*>(&action);
  if (bronchoconstr != nullptr)
  {
    GetBronchoconstriction().Copy(*bronchoconstr, true);
    m_Bronchoconstriction->Activate();
    if (!m_Bronchoconstriction->IsActive())
      RemoveBronchoconstriction();
    return true;
  }

  const SECardiovascularMechanicsModification* cmm = dynamic_cast<const SECardiovascularMechanicsModification*>(&action);
  if (cmm != nullptr)
  {
    GetCardiovascularMechanicsModification().Copy(*cmm, true);
    m_CardiovascularMechanicsModification->Activate();
    if (!m_CardiovascularMechanicsModification->IsActive())
      RemoveCardiovascularMechanicsModification();
    return true;
  }

  const SEChestCompression* cprCompression = dynamic_cast<const SEChestCompression*>(&action);
  if (cprCompression != nullptr)
  {
    if (HasChestCompressionInstantaneous())
    {
      Warning("Replacing active ChestCompressionInstantaneous action with this ChestCompression action");
    }
    if (HasChestCompression())
    {
      Warning("Current ChestCompression action has not completed, starting a new compression");
    }
    if (HasChestCompressionAutomated())
    {
      Warning("Replacing active ChestCompressionAutomated action with this ChestCompression action");
    }
    RemoveChestCompressionAutomated();
    RemoveChestCompressionInstantaneous();
    GetChestCompression().Copy(*cprCompression, true);
    m_ChestCompression->Activate();
    if (!m_ChestCompression->IsActive())
      RemoveChestCompression();
    return true;
  }

  const SEChestCompressionAutomated* cprAutomated = dynamic_cast<const SEChestCompressionAutomated*>(&action);
  if (cprAutomated != nullptr)
  {
    if (HasChestCompression())
    {
      Warning("Replacing active ChestCompression action with this ChestCompressionAutomated action");
    }
    if (HasChestCompressionInstantaneous())
    {
      Warning("Replacing active ChestCompressionInstantaneous action with this ChestCompressionAutomated action");
    }
    RemoveChestCompression();
    RemoveChestCompressionInstantaneous();
    GetChestCompressionAutomated().Copy(*cprAutomated, true);
    m_ChestCompressionAutomated->Activate();
    if (!m_ChestCompressionAutomated->IsActive())
      RemoveChestCompressionAutomated();
    return true;
  }

  const SEChestCompressionInstantaneous* cprInstantaneous = dynamic_cast<const SEChestCompressionInstantaneous*>(&action);
  if (cprInstantaneous != nullptr)
  {
    if (HasChestCompression())
    {
      Warning("Replacing active ChestCompression action with this ChestCompressionInstantaneous action");
    }
    if (HasChestCompressionAutomated())
    {
      Warning("Replacing active ChestCompressionAutomated action with this ChestCompressionInstantaneous action");
    }
    RemoveChestCompression();
    RemoveChestCompressionAutomated();
    GetChestCompressionInstantaneous().Copy(*cprInstantaneous, true);
    m_ChestCompressionInstantaneous->Activate();
    if (!m_ChestCompressionInstantaneous->IsActive())
      RemoveChestCompressionInstantaneous();
    return true;
  }

  const SEChestOcclusiveDressing* chestOccl = dynamic_cast<const SEChestOcclusiveDressing*>(&action);
  if (chestOccl != nullptr)
  {
    if (chestOccl->GetSide() == eSide::Left)
    {
      GetLeftChestOcclusiveDressing().Copy(*chestOccl, true);
      m_LeftChestOcclusiveDressing->Activate();
      if (!m_LeftChestOcclusiveDressing->IsActive())
        RemoveLeftChestOcclusiveDressing();
      return true;
    }
    else if (chestOccl->GetSide() == eSide::Right)
    {
      GetRightChestOcclusiveDressing().Copy(*chestOccl, true);
      m_RightChestOcclusiveDressing->Activate();
      if (!m_RightChestOcclusiveDressing->IsActive())
        RemoveRightChestOcclusiveDressing();
      return true;
    }
  }

  const SEChronicObstructivePulmonaryDiseaseExacerbation* copd = dynamic_cast<const SEChronicObstructivePulmonaryDiseaseExacerbation*>(&action);
  if (copd != nullptr)
  {
    GetChronicObstructivePulmonaryDiseaseExacerbation().Copy(*copd, true);
    m_COPDExacerbation->Activate();
    if (!m_COPDExacerbation->IsActive())
      RemoveChronicObstructivePulmonaryDiseaseExacerbation();
    return true;
  }

  const SEConsciousRespiration* conResp = dynamic_cast<const SEConsciousRespiration*>(&action);
  if (conResp != nullptr)
  {
    if (m_ConsciousRespiration != nullptr && m_ConsciousRespiration->HasCommands())
    {
      Warning("Processing conscious respiration before the previous conscious breath was completed.");
      Warning("Previous conscious respiration commands will not be processed.");
    }
    GetConsciousRespiration().Copy(*conResp, true);
    m_ConsciousRespiration->Activate();
    if (!m_ConsciousRespiration->IsActive())
      RemoveConsciousRespiration();
    return true;
  }

  const SEConsumeNutrients* consume = dynamic_cast<const SEConsumeNutrients*>(&action);
  if (consume != nullptr)
  {
    GetConsumeNutrients().Copy(*consume, true);
    m_ConsumeNutrients->Activate();
    if (!m_ConsumeNutrients->IsActive())
      RemoveConsumeNutrients();
    return true;
  }

  const SEDyspnea* Dyspnea = dynamic_cast<const SEDyspnea*>(&action);
  if (Dyspnea != nullptr)
  {
    GetDyspnea().Copy(*Dyspnea, true);
    m_Dyspnea->Activate();
    if (!m_Dyspnea->IsActive())
      RemoveDyspnea();
    return true;
  }

  const SEExercise* exercise = dynamic_cast<const SEExercise*>(&action);
  if (exercise != nullptr)
  {
    GetExercise().Copy(*exercise, true);
    m_Exercise->Activate();
    if (!m_Exercise->IsActive())
      RemoveExercise();
    return true;
  }

  const SEHemorrhage* hem = dynamic_cast<const SEHemorrhage*>(&action);
  if (hem != nullptr)
  {
    SEHemorrhage& mine = GetHemorrhage(hem->GetCompartment());
    mine.Copy(*hem, true);
    mine.Activate();
    if (!mine.IsActive())
      RemoveHemorrhage(hem->GetCompartment());
    return true;
  }

  const SEHemothorax* hemo = dynamic_cast<const SEHemothorax*>(&action);
  if (hemo != nullptr)
  {
    if (hemo->GetSide() == eSide::Left)
    {
      GetLeftHemothorax().Copy(*hemo, true);
      m_LeftHemothorax->Activate();
      if (!m_LeftHemothorax->IsActive())
        RemoveLeftHemothorax();
      return true;
    }
    else if (hemo->GetSide() == eSide::Right)
    {
      GetRightHemothorax().Copy(*hemo, true);
      m_RightHemothorax->Activate();
      if (!m_RightHemothorax->IsActive())
        RemoveRightHemothorax();
      return true;
    }
    Error("Unknown Hemothorax Type");
    return false;// Duno what this is...
  }

  const SEImpairedAlveolarExchangeExacerbation* imaee = dynamic_cast<const SEImpairedAlveolarExchangeExacerbation*>(&action);
  if (imaee != nullptr)
  {
    GetImpairedAlveolarExchangeExacerbation().Copy(*imaee, true);
    m_ImpairedAlveolarExchangeExacerbation->Activate();
    if (!m_ImpairedAlveolarExchangeExacerbation->IsActive())
      RemoveImpairedAlveolarExchangeExacerbation();
    return true;
  }

  const SEIntubation* intubation = dynamic_cast<const SEIntubation*>(&action);
  if (intubation != nullptr)
  {
    GetIntubation().Copy(*intubation, true);
    m_Intubation->Activate();
    if (!m_Intubation->IsActive())
      RemoveIntubation();
    return true;
  }

  const SEMechanicalVentilation* mvData = dynamic_cast<const SEMechanicalVentilation*>(&action);
  if (mvData != nullptr)
  {
    GetMechanicalVentilation().Copy(*mvData, m_SubMgr, true);
    m_MechanicalVentilation->Activate();
    if (!m_MechanicalVentilation->IsActive())
      RemoveMechanicalVentilation();
    return true;
  }

  const SENeedleDecompression* needleDecomp = dynamic_cast<const SENeedleDecompression*>(&action);
  if (needleDecomp != nullptr)
  {
    if (needleDecomp->GetSide() == eSide::Left)
    {
      GetLeftNeedleDecompression().Copy(*needleDecomp, true);
      m_LeftNeedleDecompression->Activate();
      if (!m_LeftNeedleDecompression->IsActive())
        RemoveLeftNeedleDecompression();
      return true;
    }
    else if (needleDecomp->GetSide() == eSide::Right)
    {
      GetRightNeedleDecompression().Copy(*needleDecomp, true);
      m_RightNeedleDecompression->Activate();
      if (!m_RightNeedleDecompression->IsActive())
        RemoveRightNeedleDecompression();
      return true;
    }
    Error("Unknown Needle Decompression Type");
    return false;
  }

  const SEPericardialEffusion* pericardialEff = dynamic_cast<const SEPericardialEffusion*>(&action);
  if (pericardialEff != nullptr)
  {
    GetPericardialEffusion().Copy(*pericardialEff, true);
    m_PericardialEffusion->Activate();
    if (!m_PericardialEffusion->IsActive())
      RemovePericardialEffusion();
    return true;
  }

  const SEPneumoniaExacerbation* lp = dynamic_cast<const SEPneumoniaExacerbation*>(&action);
  if (lp != nullptr)
  {
    GetPneumoniaExacerbation().Copy(*lp, true);
    m_PneumoniaExacerbation->Activate();
    if (!m_PneumoniaExacerbation->IsActive())
      RemovePneumoniaExacerbation();
    return true;
  }

  const SEPulmonaryShuntExacerbation* pulshuntex = dynamic_cast<const SEPulmonaryShuntExacerbation*>(&action);
  if (pulshuntex != nullptr)
  {
    GetPulmonaryShuntExacerbation().Copy(*pulshuntex, true);
      m_PulmonaryShuntExacerbation->Activate();
    if (!m_PulmonaryShuntExacerbation->IsActive())
      RemovePulmonaryShuntExacerbation();
    return true;
  }

  const SERespiratoryFatigue* rf = dynamic_cast<const SERespiratoryFatigue*>(&action);
  if (rf != nullptr)
  {
    GetRespiratoryFatigue().Copy(*rf, true);
    m_RespiratoryFatigue->Activate();
    if (!m_RespiratoryFatigue->IsActive())
      RemoveRespiratoryFatigue();
    return true;
  }

  const SERespiratoryMechanicsConfiguration* rmc = dynamic_cast<const SERespiratoryMechanicsConfiguration*>(&action);
  if (rmc != nullptr)
  {
    GetRespiratoryMechanicsConfiguration().Copy(*rmc, true);
    m_RespiratoryMechanicsConfiguration->Activate();
    if (!m_RespiratoryMechanicsConfiguration->IsActive())
      RemoveRespiratoryMechanicsConfiguration();
    return true;
  }

  const SERespiratoryMechanicsModification* rmm = dynamic_cast<const SERespiratoryMechanicsModification*>(&action);
  if (rmm != nullptr)
  {
    GetRespiratoryMechanicsModification().Copy(*rmm, true);
    m_RespiratoryMechanicsModification->Activate();
    if (!m_RespiratoryMechanicsModification->IsActive())
      RemoveRespiratoryMechanicsModification();
    return true;
  }

  const SESubstanceBolus* bolus = dynamic_cast<const SESubstanceBolus*>(&action);
  if (bolus != nullptr)
  {
    const SESubstance* sub = m_SubMgr.GetSubstance(bolus->GetSubstance().GetName());
    if (sub == nullptr)//Make sure this substance manager has it
    {
      Error("Ignoring SESubstanceBolus action due to unknown substance in action: " + bolus->GetSubstance().GetName());
      return false;
    }
    SESubstanceBolus& mine = GetSubstanceBolus(*sub);
    mine.Copy(*bolus, true);
    mine.Activate();
    if (!mine.IsActive())
      RemoveSubstanceBolus(*sub);
    else
      m_SubMgr.AddActiveSubstance(*sub);
    return true;
  }

  const SESubstanceInfusion* subInfusion = dynamic_cast<const SESubstanceInfusion*>(&action);
  if (subInfusion != nullptr)
  {
    const SESubstance* sub = m_SubMgr.GetSubstance(subInfusion->GetSubstance().GetName());
    if (sub == nullptr)//Make sure this substance manager has it
    {
      Error("Ignoring SESubstanceInfusion action due to unknown substance in action: " + subInfusion->GetSubstance().GetName());
      return false;
    }
    SESubstanceInfusion& mine = GetSubstanceInfusion(*sub);
    mine.Copy(*subInfusion, true);
    mine.Activate();
    if (!mine.IsActive())
      RemoveSubstanceInfusion(*sub);
    else
      m_SubMgr.AddActiveSubstance(*sub);
    return true;
  }

  const SESubstanceCompoundInfusion* cSubInfusion = dynamic_cast<const SESubstanceCompoundInfusion*>(&action);
  if (cSubInfusion != nullptr)
  {
    const SESubstanceCompound* cmpd = m_SubMgr.GetCompound(cSubInfusion->GetSubstanceCompound().GetName());
    if (cmpd == nullptr)//Make sure this substance manager has it
    {
      Error("Ignoring SESubstanceCompoundInfusion action due to unknown substance in action: " + cSubInfusion->GetSubstanceCompound().GetName());
      return false;
    }
    SESubstanceCompoundInfusion& mine = GetSubstanceCompoundInfusion(*cmpd);
    mine.Copy(*cSubInfusion, true);
    mine.Activate();
    if (!mine.IsActive())
      RemoveSubstanceCompoundInfusion(*cmpd);
    else
    {
      m_SubMgr.AddActiveCompound(*cmpd);
      for (const SESubstanceConcentration* scc : cmpd->GetComponents())
      {
        m_SubMgr.AddActiveSubstance(scc->GetSubstance());
      }
    }
    return true;
  }

  const SESupplementalOxygen* supplementalO2 = dynamic_cast<const SESupplementalOxygen*>(&action);
  if (supplementalO2 != nullptr)
  {
    GetSupplementalOxygen().Copy(*supplementalO2, true);
    m_SupplementalOxygen->Activate();
    if (!m_SupplementalOxygen->IsActive())
      RemoveSupplementalOxygen();
    return true;
  }

  const SETensionPneumothorax* pneumo = dynamic_cast<const SETensionPneumothorax*>(&action);
  if (pneumo != nullptr)
  {
    if (pneumo->GetSide() == eSide::Left&&pneumo->GetType() == eGate::Open)
    {
      GetLeftOpenTensionPneumothorax().Copy(*pneumo, true);
      m_LeftOpenTensionPneumothorax->Activate();
      if (!m_LeftOpenTensionPneumothorax->IsActive())
        RemoveLeftOpenTensionPneumothorax();
      return true;
    }
    else if (pneumo->GetSide() == eSide::Left&&pneumo->GetType() == eGate::Closed)
    {
      GetLeftClosedTensionPneumothorax().Copy(*pneumo, true);
      m_LeftClosedTensionPneumothorax->Activate();
      if (!m_LeftClosedTensionPneumothorax->IsActive())
        RemoveLeftClosedTensionPneumothorax();
      return true;
    }
    else if (pneumo->GetSide() == eSide::Right&&pneumo->GetType() == eGate::Open)
    {
      GetRightOpenTensionPneumothorax().Copy(*pneumo, true);
      m_RightOpenTensionPneumothorax->Activate();
      if (!m_RightOpenTensionPneumothorax->IsActive())
        RemoveRightOpenTensionPneumothorax();
      return true;
    }
    else if (pneumo->GetSide() == eSide::Right&&pneumo->GetType() == eGate::Closed)
    {
      GetRightClosedTensionPneumothorax().Copy(*pneumo, true);
      m_RightClosedTensionPneumothorax->Activate();
      if (!m_RightClosedTensionPneumothorax->IsActive())
        RemoveRightClosedTensionPneumothorax();
      return true;
    }
    Error("Unknown Tension Pnumothorax Type");
    return false;// Duno what this is...
  }

  const SETubeThoracostomy* tubeThora = dynamic_cast<const SETubeThoracostomy*>(&action);
  if (tubeThora != nullptr)
  {
    if (tubeThora->GetSide() == eSide::Left)
    {
      GetLeftTubeThoracostomy().Copy(*tubeThora, true);
      m_LeftTubeThoracostomy->Activate();
      if (!m_LeftTubeThoracostomy->IsActive())
        RemoveLeftTubeThoracostomy();
      return true;
    }
    else if (tubeThora->GetSide() == eSide::Right)
    {
      GetRightTubeThoracostomy().Copy(*tubeThora, true);
      m_RightTubeThoracostomy->Activate();
      if (!m_RightTubeThoracostomy->IsActive())
        RemoveRightTubeThoracostomy();
      return true;
    }
    Error("Unknown Tube Thoracostomy Type");
    return false;// Duno what this is...
  }

  const SEUrinate* urinate = dynamic_cast<const SEUrinate*>(&action);
  if (urinate != nullptr)
  {
    GetUrinate().Copy(*urinate, true);
    m_Urinate->Activate();
    if (!m_Urinate->IsActive())
      RemoveUrinate();
    return true;
  }

  Error("Unsupported Action");
  return false;
}

bool SEPatientActionCollection::HasAcuteRespiratoryDistressSyndromeExacerbation() const
{
  return m_ARDSExacerbation == nullptr ? false : m_ARDSExacerbation->IsActive();
}
SEAcuteRespiratoryDistressSyndromeExacerbation& SEPatientActionCollection::GetAcuteRespiratoryDistressSyndromeExacerbation()
{
  if (m_ARDSExacerbation == nullptr)
    m_ARDSExacerbation = new SEAcuteRespiratoryDistressSyndromeExacerbation(GetLogger());
  return *m_ARDSExacerbation;
}
const SEAcuteRespiratoryDistressSyndromeExacerbation* SEPatientActionCollection::GetAcuteRespiratoryDistressSyndromeExacerbation() const
{
  return m_ARDSExacerbation;
}
void SEPatientActionCollection::RemoveAcuteRespiratoryDistressSyndromeExacerbation()
{
  if(m_ARDSExacerbation)
    m_ARDSExacerbation->Deactivate();
}

bool SEPatientActionCollection::HasAcuteStress() const
{
  return m_AcuteStress == nullptr ? false : m_AcuteStress->IsActive();
}
SEAcuteStress& SEPatientActionCollection::GetAcuteStress()
{
  if (m_AcuteStress == nullptr)
    m_AcuteStress = new SEAcuteStress(GetLogger());
  return *m_AcuteStress;
}
const SEAcuteStress* SEPatientActionCollection::GetAcuteStress() const
{
  return m_AcuteStress;
}
void SEPatientActionCollection::RemoveAcuteStress()
{
  if (m_AcuteStress)
    m_AcuteStress->Deactivate();
}

bool SEPatientActionCollection::HasAirwayObstruction() const
{
  return m_AirwayObstruction == nullptr ? false : m_AirwayObstruction->IsActive();
}
SEAirwayObstruction& SEPatientActionCollection::GetAirwayObstruction()
{
  if (m_AirwayObstruction == nullptr)
    m_AirwayObstruction = new SEAirwayObstruction(GetLogger());
  return *m_AirwayObstruction;
}
const SEAirwayObstruction* SEPatientActionCollection::GetAirwayObstruction() const
{
  return m_AirwayObstruction;
}
void SEPatientActionCollection::RemoveAirwayObstruction()
{
  if (m_AirwayObstruction)
    m_AirwayObstruction->Deactivate();
}

bool SEPatientActionCollection::HasArrhythmia() const
{
  return m_Arrhythmia == nullptr ? false : m_Arrhythmia->IsActive();
}
SEArrhythmia& SEPatientActionCollection::GetArrhythmia()
{
  if (m_Arrhythmia == nullptr)
    m_Arrhythmia = new SEArrhythmia(GetLogger());
  return *m_Arrhythmia;
}
const SEArrhythmia* SEPatientActionCollection::GetArrhythmia() const
{
  return m_Arrhythmia;
}
void SEPatientActionCollection::RemoveArrhythmia()
{
  if (m_Arrhythmia)
    m_Arrhythmia->Deactivate();
}

bool SEPatientActionCollection::HasAsthmaAttack() const
{
  return m_AsthmaAttack == nullptr ? false : m_AsthmaAttack->IsActive();
}
SEAsthmaAttack& SEPatientActionCollection::GetAsthmaAttack()
{
  if (m_AsthmaAttack == nullptr)
    m_AsthmaAttack = new SEAsthmaAttack(GetLogger());
  return *m_AsthmaAttack;
}
const SEAsthmaAttack* SEPatientActionCollection::GetAsthmaAttack() const
{
  return m_AsthmaAttack;
}
void SEPatientActionCollection::RemoveAsthmaAttack()
{
  if (m_AsthmaAttack)
    m_AsthmaAttack->Deactivate();
}

bool SEPatientActionCollection::HasBrainInjury() const
{
  return m_BrainInjury == nullptr ? false : m_BrainInjury->IsActive();
}
SEBrainInjury& SEPatientActionCollection::GetBrainInjury()
{
  if (m_BrainInjury == nullptr)
    m_BrainInjury = new SEBrainInjury(GetLogger());
  return *m_BrainInjury;
}
const SEBrainInjury* SEPatientActionCollection::GetBrainInjury() const
{
  return m_BrainInjury;
}
void SEPatientActionCollection::RemoveBrainInjury()
{
  if (m_BrainInjury)
    m_BrainInjury->Deactivate();
}

bool SEPatientActionCollection::HasBronchoconstriction() const
{
  return m_Bronchoconstriction == nullptr ? false : m_Bronchoconstriction->IsActive();
}
SEBronchoconstriction& SEPatientActionCollection::GetBronchoconstriction()
{
  if (m_Bronchoconstriction == nullptr)
    m_Bronchoconstriction = new SEBronchoconstriction(GetLogger());
  return *m_Bronchoconstriction;
}
const SEBronchoconstriction* SEPatientActionCollection::GetBronchoconstriction() const
{
  return m_Bronchoconstriction;
}
void SEPatientActionCollection::RemoveBronchoconstriction()
{
  if (m_Bronchoconstriction)
    m_Bronchoconstriction->Deactivate();
}

bool SEPatientActionCollection::HasCardiovascularMechanicsModification() const
{
  return m_CardiovascularMechanicsModification == nullptr ? false : m_CardiovascularMechanicsModification->IsActive();
}
SECardiovascularMechanicsModification& SEPatientActionCollection::GetCardiovascularMechanicsModification()
{
  if (m_CardiovascularMechanicsModification == nullptr)
    m_CardiovascularMechanicsModification = new SECardiovascularMechanicsModification(GetLogger());
  return *m_CardiovascularMechanicsModification;
}
const SECardiovascularMechanicsModification* SEPatientActionCollection::GetCardiovascularMechanicsModification() const
{
  return m_CardiovascularMechanicsModification;
}
void SEPatientActionCollection::RemoveCardiovascularMechanicsModification()
{
  if (m_CardiovascularMechanicsModification)
    m_CardiovascularMechanicsModification->Deactivate();
}

bool SEPatientActionCollection::HasActiveCPRAction() const
{
  return HasChestCompression() || HasChestCompressionAutomated() || HasChestCompressionInstantaneous();
}

bool SEPatientActionCollection::HasChestCompression() const
{
  return m_ChestCompression == nullptr ? false : m_ChestCompression->IsActive();
}
SEChestCompression& SEPatientActionCollection::GetChestCompression()
{
  if (m_ChestCompression == nullptr)
    m_ChestCompression = new SEChestCompression(GetLogger());
  return *m_ChestCompression;
}
const SEChestCompression* SEPatientActionCollection::GetChestCompression() const
{
  return m_ChestCompression;
}
void SEPatientActionCollection::RemoveChestCompression()
{
  if (m_ChestCompression)
    m_ChestCompression->Deactivate();
}

bool SEPatientActionCollection::HasChestCompressionAutomated() const
{
  return m_ChestCompressionAutomated == nullptr ? false : m_ChestCompressionAutomated->IsActive();
}
SEChestCompressionAutomated& SEPatientActionCollection::GetChestCompressionAutomated()
{
  if (m_ChestCompressionAutomated == nullptr)
    m_ChestCompressionAutomated = new SEChestCompressionAutomated(GetLogger());
  return *m_ChestCompressionAutomated;
}
const SEChestCompressionAutomated* SEPatientActionCollection::GetChestCompressionAutomated() const
{
  return m_ChestCompressionAutomated;
}
void SEPatientActionCollection::RemoveChestCompressionAutomated()
{
  if (m_ChestCompressionAutomated)
    m_ChestCompressionAutomated->Deactivate();
}

bool SEPatientActionCollection::HasChestCompressionInstantaneous() const
{
  return m_ChestCompressionInstantaneous == nullptr ? false : m_ChestCompressionInstantaneous->IsActive();
}
SEChestCompressionInstantaneous& SEPatientActionCollection::GetChestCompressionInstantaneous()
{
  if (m_ChestCompressionInstantaneous == nullptr)
    m_ChestCompressionInstantaneous = new SEChestCompressionInstantaneous(GetLogger());
  return *m_ChestCompressionInstantaneous;
}
const SEChestCompressionInstantaneous* SEPatientActionCollection::GetChestCompressionInstantaneous() const
{
  return m_ChestCompressionInstantaneous;
}
void SEPatientActionCollection::RemoveChestCompressionInstantaneous()
{
  if (m_ChestCompressionInstantaneous)
    m_ChestCompressionInstantaneous->Deactivate();
}

bool SEPatientActionCollection::HasChestOcclusiveDressing() const
{
  return HasLeftChestOcclusiveDressing() || HasRightChestOcclusiveDressing();
}
bool SEPatientActionCollection::HasLeftChestOcclusiveDressing() const
{
  return m_LeftChestOcclusiveDressing == nullptr ? false : m_LeftChestOcclusiveDressing->IsActive();
}
SEChestOcclusiveDressing& SEPatientActionCollection::GetLeftChestOcclusiveDressing()
{
  if (m_LeftChestOcclusiveDressing == nullptr)
    m_LeftChestOcclusiveDressing = new SEChestOcclusiveDressing(GetLogger());
  return *m_LeftChestOcclusiveDressing;
}
const SEChestOcclusiveDressing* SEPatientActionCollection::GetLeftChestOcclusiveDressing() const
{
  return m_LeftChestOcclusiveDressing;
}
void SEPatientActionCollection::RemoveLeftChestOcclusiveDressing()
{
  if (m_LeftChestOcclusiveDressing)
    m_LeftChestOcclusiveDressing->Deactivate();
}
bool SEPatientActionCollection::HasRightChestOcclusiveDressing() const
{
  return m_RightChestOcclusiveDressing == nullptr ? false : m_RightChestOcclusiveDressing->IsActive();
}
SEChestOcclusiveDressing& SEPatientActionCollection::GetRightChestOcclusiveDressing()
{
  if (m_RightChestOcclusiveDressing == nullptr)
    m_RightChestOcclusiveDressing = new SEChestOcclusiveDressing(GetLogger());
  return *m_RightChestOcclusiveDressing;
}
const SEChestOcclusiveDressing* SEPatientActionCollection::GetRightChestOcclusiveDressing() const
{
  return m_RightChestOcclusiveDressing;
}
void SEPatientActionCollection::RemoveRightChestOcclusiveDressing()
{
  if (m_RightChestOcclusiveDressing)
    m_RightChestOcclusiveDressing->Deactivate();
}

bool SEPatientActionCollection::HasChronicObstructivePulmonaryDiseaseExacerbation() const
{
  return m_COPDExacerbation == nullptr ? false : m_COPDExacerbation->IsActive();
}
SEChronicObstructivePulmonaryDiseaseExacerbation& SEPatientActionCollection::GetChronicObstructivePulmonaryDiseaseExacerbation()
{
  if (m_COPDExacerbation == nullptr)
    m_COPDExacerbation = new SEChronicObstructivePulmonaryDiseaseExacerbation(GetLogger());
  return *m_COPDExacerbation;
}
const SEChronicObstructivePulmonaryDiseaseExacerbation* SEPatientActionCollection::GetChronicObstructivePulmonaryDiseaseExacerbation() const
{
  return m_COPDExacerbation;
}
void SEPatientActionCollection::RemoveChronicObstructivePulmonaryDiseaseExacerbation()
{
  if (m_COPDExacerbation)
    m_COPDExacerbation->Deactivate();
}

bool SEPatientActionCollection::HasConsciousRespiration() const
{
  return m_ConsciousRespiration == nullptr ? false : m_ConsciousRespiration->IsActive();
}
SEConsciousRespiration& SEPatientActionCollection::GetConsciousRespiration()
{
  if (m_ConsciousRespiration == nullptr)
    m_ConsciousRespiration = new SEConsciousRespiration(GetLogger());
  return *m_ConsciousRespiration;
}
const SEConsciousRespiration* SEPatientActionCollection::GetConsciousRespiration() const
{
  return m_ConsciousRespiration;
}
void SEPatientActionCollection::RemoveConsciousRespiration()
{
  if (m_ConsciousRespiration)
    m_ConsciousRespiration->Deactivate();
}

bool SEPatientActionCollection::HasConsumeNutrients() const
{
  return m_ConsumeNutrients == nullptr ? false : m_ConsumeNutrients->IsActive();
}
SEConsumeNutrients& SEPatientActionCollection::GetConsumeNutrients()
{
  if (m_ConsumeNutrients == nullptr)
    m_ConsumeNutrients = new SEConsumeNutrients(GetLogger());
  return *m_ConsumeNutrients;
}
const SEConsumeNutrients* SEPatientActionCollection::GetConsumeNutrients() const
{
  return m_ConsumeNutrients;
}
void SEPatientActionCollection::RemoveConsumeNutrients()
{
  if (m_ConsumeNutrients)
    m_ConsumeNutrients->Deactivate();
}

bool SEPatientActionCollection::HasDyspnea() const
{
  return m_Dyspnea == nullptr ? false : m_Dyspnea->IsActive();
}
SEDyspnea& SEPatientActionCollection::GetDyspnea()
{
  if (m_Dyspnea == nullptr)
    m_Dyspnea = new SEDyspnea(GetLogger());
  return *m_Dyspnea;
}
const SEDyspnea* SEPatientActionCollection::GetDyspnea() const
{
  return m_Dyspnea;
}
void SEPatientActionCollection::RemoveDyspnea()
{
  if (m_Dyspnea)
    m_Dyspnea->Deactivate();
}

bool SEPatientActionCollection::HasExercise() const
{
  return m_Exercise == nullptr ? false : m_Exercise->IsActive();
}
SEExercise& SEPatientActionCollection::GetExercise()
{
  if (m_Exercise == nullptr)
    m_Exercise = new SEExercise(GetLogger());
  return *m_Exercise;
}
const SEExercise* SEPatientActionCollection::GetExercise() const
{
  return m_Exercise;
}
void SEPatientActionCollection::RemoveExercise()
{
  if (m_Exercise)
    m_Exercise->Deactivate();
}

bool SEPatientActionCollection::HasHemorrhage() const
{
  for (auto h : m_Hemorrhages)
    if (h->IsActive())
      return true;
  return false;
}
bool SEPatientActionCollection::HasHemorrhage(eHemorrhage_Compartment cmpt) const
{
  return GetHemorrhage(cmpt)!=nullptr;
}
SEHemorrhage& SEPatientActionCollection::GetHemorrhage(eHemorrhage_Compartment cmpt)
{
  for (auto h : m_Hemorrhages)
    if (h->GetCompartment() == cmpt)
      return *h;
  SEHemorrhage* h = new SEHemorrhage(GetLogger());
  h->SetCompartment(cmpt);
  m_Hemorrhages.push_back(h);
  return *h;
}
const SEHemorrhage* SEPatientActionCollection::GetHemorrhage(eHemorrhage_Compartment cmpt) const
{
  for (auto h : m_Hemorrhages)
    if (h->GetCompartment() == cmpt)
      return h;
  return nullptr;
}
const std::vector<SEHemorrhage*>& SEPatientActionCollection::GetHemorrhages()
{
  return m_Hemorrhages;
}
const std::vector<const SEHemorrhage*> SEPatientActionCollection::GetHemorrhages() const
{
  return std::vector<const SEHemorrhage*>(m_Hemorrhages.begin(), m_Hemorrhages.end());
}
void SEPatientActionCollection::RemoveHemorrhage(eHemorrhage_Compartment cmpt)
{
  GetHemorrhage(cmpt).Deactivate();
}

bool SEPatientActionCollection::HasHemothorax() const
{
  if (m_LeftHemothorax != nullptr&&m_LeftHemothorax->IsActive())
    return true;
  if (m_RightHemothorax != nullptr&&m_RightHemothorax->IsActive())
    return true;
  return false;
}
bool SEPatientActionCollection::HasLeftHemothorax() const
{
  if (m_LeftHemothorax != nullptr&&m_LeftHemothorax->IsActive())
    return true;
  return false;
}
SEHemothorax& SEPatientActionCollection::GetLeftHemothorax()
{
  if (m_LeftHemothorax == nullptr)
    m_LeftHemothorax = new SEHemothorax(GetLogger());
  return *m_LeftHemothorax;
}
const SEHemothorax* SEPatientActionCollection::GetLeftHemothorax() const
{
  return m_LeftHemothorax;
}
void SEPatientActionCollection::RemoveLeftHemothorax()
{
  if (m_LeftHemothorax)
    m_LeftHemothorax->Deactivate();
}
bool SEPatientActionCollection::HasRightHemothorax() const
{
  if (m_RightHemothorax != nullptr&&m_RightHemothorax->IsActive())
    return true;
  return false;
}
SEHemothorax& SEPatientActionCollection::GetRightHemothorax()
{
  if (m_RightHemothorax == nullptr)
    m_RightHemothorax = new SEHemothorax(GetLogger());
  return *m_RightHemothorax;
}
const SEHemothorax* SEPatientActionCollection::GetRightHemothorax() const
{
  return m_RightHemothorax;
}
void SEPatientActionCollection::RemoveRightHemothorax()
{
  if (m_RightHemothorax)
    m_RightHemothorax->Deactivate();
}

bool SEPatientActionCollection::HasImpairedAlveolarExchangeExacerbation() const
{
  return m_ImpairedAlveolarExchangeExacerbation == nullptr ? false : m_ImpairedAlveolarExchangeExacerbation->IsActive();
}
SEImpairedAlveolarExchangeExacerbation& SEPatientActionCollection::GetImpairedAlveolarExchangeExacerbation()
{
  if (m_ImpairedAlveolarExchangeExacerbation == nullptr)
    m_ImpairedAlveolarExchangeExacerbation = new SEImpairedAlveolarExchangeExacerbation(GetLogger());
  return *m_ImpairedAlveolarExchangeExacerbation;
}
const SEImpairedAlveolarExchangeExacerbation* SEPatientActionCollection::GetImpairedAlveolarExchangeExacerbation() const
{
  return m_ImpairedAlveolarExchangeExacerbation;
}
void SEPatientActionCollection::RemoveImpairedAlveolarExchangeExacerbation()
{
  if (m_ImpairedAlveolarExchangeExacerbation)
    m_ImpairedAlveolarExchangeExacerbation->Deactivate();
}

bool SEPatientActionCollection::HasIntubation() const
{
  return m_Intubation == nullptr ? false : m_Intubation->IsActive();
}
SEIntubation& SEPatientActionCollection::GetIntubation()
{
  if (m_Intubation == nullptr)
    m_Intubation = new SEIntubation(GetLogger());
  return *m_Intubation;
}
const SEIntubation* SEPatientActionCollection::GetIntubation() const
{
  return m_Intubation;
}
void SEPatientActionCollection::RemoveIntubation()
{
  if (m_Intubation)
    m_Intubation->Deactivate();
}

bool SEPatientActionCollection::HasMechanicalVentilation() const
{
  return m_MechanicalVentilation == nullptr ? false : m_MechanicalVentilation->IsActive();
}
SEMechanicalVentilation& SEPatientActionCollection::GetMechanicalVentilation()
{
  if (m_MechanicalVentilation == nullptr)
    m_MechanicalVentilation = new SEMechanicalVentilation(GetLogger());
  return *m_MechanicalVentilation;
}
const SEMechanicalVentilation* SEPatientActionCollection::GetMechanicalVentilation() const
{
  return m_MechanicalVentilation;
}
void SEPatientActionCollection::RemoveMechanicalVentilation()
{
  if (m_MechanicalVentilation)
    m_MechanicalVentilation->Deactivate();
}

bool SEPatientActionCollection::HasNeedleDecompression() const
{
  return HasLeftNeedleDecompression() || HasRightNeedleDecompression();
}
bool SEPatientActionCollection::HasLeftNeedleDecompression() const
{
  return m_LeftNeedleDecompression == nullptr ? false : m_LeftNeedleDecompression->IsActive();
}
SENeedleDecompression& SEPatientActionCollection::GetLeftNeedleDecompression()
{
  if (m_LeftNeedleDecompression == nullptr)
    m_LeftNeedleDecompression = new SENeedleDecompression(GetLogger());
  return *m_LeftNeedleDecompression;
}
const SENeedleDecompression* SEPatientActionCollection::GetLeftNeedleDecompression() const
{
  return m_LeftNeedleDecompression;
}
void SEPatientActionCollection::RemoveLeftNeedleDecompression()
{
  if (m_LeftNeedleDecompression)
    m_LeftNeedleDecompression->Deactivate();
}
bool SEPatientActionCollection::HasRightNeedleDecompression() const
{
  return m_RightNeedleDecompression == nullptr ? false : m_RightNeedleDecompression->IsActive();
}
SENeedleDecompression& SEPatientActionCollection::GetRightNeedleDecompression()
{
  if (m_RightNeedleDecompression == nullptr)
    m_RightNeedleDecompression = new SENeedleDecompression(GetLogger());
  return *m_RightNeedleDecompression;
}
const SENeedleDecompression* SEPatientActionCollection::GetRightNeedleDecompression() const
{
  return m_RightNeedleDecompression;
}
void SEPatientActionCollection::RemoveRightNeedleDecompression()
{
  if (m_RightNeedleDecompression)
    m_RightNeedleDecompression->Deactivate();
}

bool SEPatientActionCollection::HasPericardialEffusion() const
{
  return m_PericardialEffusion == nullptr ? false : m_PericardialEffusion->IsActive();
}
SEPericardialEffusion& SEPatientActionCollection::GetPericardialEffusion()
{
  if (m_PericardialEffusion == nullptr)
    m_PericardialEffusion = new SEPericardialEffusion(GetLogger());
  return *m_PericardialEffusion;
}
const SEPericardialEffusion* SEPatientActionCollection::GetPericardialEffusion() const
{
  return m_PericardialEffusion;
}
void SEPatientActionCollection::RemovePericardialEffusion()
{
  if (m_PericardialEffusion)
    m_PericardialEffusion->Deactivate();
}

bool SEPatientActionCollection::HasPneumoniaExacerbation() const
{
  return m_PneumoniaExacerbation == nullptr ? false : m_PneumoniaExacerbation->IsActive();
}
SEPneumoniaExacerbation& SEPatientActionCollection::GetPneumoniaExacerbation()
{
  if (m_PneumoniaExacerbation == nullptr)
    m_PneumoniaExacerbation = new SEPneumoniaExacerbation(GetLogger());
  return *m_PneumoniaExacerbation;
}
const SEPneumoniaExacerbation* SEPatientActionCollection::GetPneumoniaExacerbation() const
{
  return m_PneumoniaExacerbation;
}
void SEPatientActionCollection::RemovePneumoniaExacerbation()
{
  if (m_PneumoniaExacerbation)
    m_PneumoniaExacerbation->Deactivate();
}

bool SEPatientActionCollection::HasPulmonaryShuntExacerbation() const
{
  return m_PulmonaryShuntExacerbation == nullptr ? false : m_PulmonaryShuntExacerbation->IsActive();
}
SEPulmonaryShuntExacerbation& SEPatientActionCollection::GetPulmonaryShuntExacerbation()
{
  if (m_PulmonaryShuntExacerbation == nullptr)
    m_PulmonaryShuntExacerbation = new SEPulmonaryShuntExacerbation(GetLogger());
  return *m_PulmonaryShuntExacerbation;
}
const SEPulmonaryShuntExacerbation* SEPatientActionCollection::GetPulmonaryShuntExacerbation() const
{
  return m_PulmonaryShuntExacerbation;
}
void SEPatientActionCollection::RemovePulmonaryShuntExacerbation()
{
  if (m_PulmonaryShuntExacerbation)
    m_PulmonaryShuntExacerbation->Deactivate();
}

bool SEPatientActionCollection::HasRespiratoryFatigue() const
{
  return m_RespiratoryFatigue == nullptr ? false : m_RespiratoryFatigue->IsActive();
}
SERespiratoryFatigue& SEPatientActionCollection::GetRespiratoryFatigue()
{
  if (m_RespiratoryFatigue == nullptr)
    m_RespiratoryFatigue = new SERespiratoryFatigue(GetLogger());
  return *m_RespiratoryFatigue;
}
const SERespiratoryFatigue* SEPatientActionCollection::GetRespiratoryFatigue() const
{
  return m_RespiratoryFatigue;
}
void SEPatientActionCollection::RemoveRespiratoryFatigue()
{
  if (m_RespiratoryFatigue)
    m_RespiratoryFatigue->Deactivate();
}

bool SEPatientActionCollection::HasRespiratoryMechanicsConfiguration() const
{
  return m_RespiratoryMechanicsConfiguration == nullptr ? false : m_RespiratoryMechanicsConfiguration->IsActive();
}
SERespiratoryMechanicsConfiguration& SEPatientActionCollection::GetRespiratoryMechanicsConfiguration()
{
  if (m_RespiratoryMechanicsConfiguration == nullptr)
    m_RespiratoryMechanicsConfiguration = new SERespiratoryMechanicsConfiguration(GetLogger());
  return *m_RespiratoryMechanicsConfiguration;
}
const SERespiratoryMechanicsConfiguration* SEPatientActionCollection::GetRespiratoryMechanicsConfiguration() const
{
  return m_RespiratoryMechanicsConfiguration;
}
void SEPatientActionCollection::RemoveRespiratoryMechanicsConfiguration()
{
  if (m_RespiratoryMechanicsConfiguration)
    m_RespiratoryMechanicsConfiguration->Deactivate();
}

bool SEPatientActionCollection::HasRespiratoryMechanicsModification() const
{
  return m_RespiratoryMechanicsModification == nullptr ? false : m_RespiratoryMechanicsModification->IsActive();
}
SERespiratoryMechanicsModification& SEPatientActionCollection::GetRespiratoryMechanicsModification()
{
  if (m_RespiratoryMechanicsModification == nullptr)
    m_RespiratoryMechanicsModification = new SERespiratoryMechanicsModification(GetLogger());
  return *m_RespiratoryMechanicsModification;
}
const SERespiratoryMechanicsModification* SEPatientActionCollection::GetRespiratoryMechanicsModification() const
{
  return m_RespiratoryMechanicsModification;
}
void SEPatientActionCollection::RemoveRespiratoryMechanicsModification()
{
  if (m_RespiratoryMechanicsModification)
    m_RespiratoryMechanicsModification->Deactivate();
}

bool SEPatientActionCollection::HasSupplementalOxygen() const
{
  return m_SupplementalOxygen == nullptr ? false : m_SupplementalOxygen->IsActive();
}
SESupplementalOxygen& SEPatientActionCollection::GetSupplementalOxygen()
{
  if (m_SupplementalOxygen == nullptr)
    m_SupplementalOxygen = new SESupplementalOxygen(GetLogger());
  return *m_SupplementalOxygen;
}
const SESupplementalOxygen* SEPatientActionCollection::GetSupplementalOxygen() const
{
  return m_SupplementalOxygen;
}
void SEPatientActionCollection::RemoveSupplementalOxygen()
{
  if (m_SupplementalOxygen)
    m_SupplementalOxygen->Deactivate();
}

bool SEPatientActionCollection::HasSubstanceBolus() const
{
  for (auto h : m_SubstanceBoluses)
    if (h->IsActive())
      return true;
  return false;
}
bool SEPatientActionCollection::HasSubstanceBolus(const SESubstance& sub) const
{
  return GetSubstanceBolus(sub) != nullptr;
}
SESubstanceBolus& SEPatientActionCollection::GetSubstanceBolus(const SESubstance& sub)
{
  for (auto b : m_SubstanceBoluses)
    if (&b->GetSubstance() == &sub)
      return *b;
  SESubstanceBolus* b = new SESubstanceBolus(sub, GetLogger());
  m_SubstanceBoluses.push_back(b);
  return *b;
}
const SESubstanceBolus* SEPatientActionCollection::GetSubstanceBolus(const SESubstance& sub) const
{
  for (auto b : m_SubstanceBoluses)
    if (&b->GetSubstance() == &sub)
      return b;
  return nullptr;
}
const std::vector<SESubstanceBolus*>& SEPatientActionCollection::GetSubstanceBoluses()
{
  return m_SubstanceBoluses;
}
const std::vector<const SESubstanceBolus*> SEPatientActionCollection::GetSubstanceBoluses() const
{
  return std::vector<const SESubstanceBolus*>(m_SubstanceBoluses.begin(), m_SubstanceBoluses.end());
}
void SEPatientActionCollection::RemoveSubstanceBolus(const SESubstance& sub)
{
  GetSubstanceBolus(sub).Deactivate();
}

bool SEPatientActionCollection::HasSubstanceInfusion() const
{
  for (auto h : m_SubstanceInfusions)
    if (h->IsActive())
      return true;
  return false;
}
bool SEPatientActionCollection::HasSubstanceInfusion(const SESubstance& sub) const
{
  return GetSubstanceInfusion(sub) != nullptr;
}
SESubstanceInfusion& SEPatientActionCollection::GetSubstanceInfusion(const SESubstance& sub)
{
  for (auto si : m_SubstanceInfusions)
    if (&si->GetSubstance() == &sub)
      return *si;
  SESubstanceInfusion* si = new SESubstanceInfusion(sub, GetLogger());
  m_SubstanceInfusions.push_back(si);
  return *si;
}
const SESubstanceInfusion* SEPatientActionCollection::GetSubstanceInfusion(const SESubstance& sub) const
{
  for (auto si : m_SubstanceInfusions)
    if (&si->GetSubstance() == &sub)
      return si;
  return nullptr;
}
const std::vector<SESubstanceInfusion*>& SEPatientActionCollection::GetSubstanceInfusions()
{
  return m_SubstanceInfusions;
}
const std::vector<const SESubstanceInfusion*> SEPatientActionCollection::GetSubstanceInfusions() const
{
  return std::vector<const SESubstanceInfusion*>(m_SubstanceInfusions.begin(), m_SubstanceInfusions.end());
}
void SEPatientActionCollection::RemoveSubstanceInfusion(const SESubstance& sub)
{
  GetSubstanceInfusion(sub).Deactivate();
}

bool SEPatientActionCollection::HasSubstanceCompoundInfusion() const
{
  for (auto h : m_SubstanceCompoundInfusions)
    if (h->IsActive())
      return true;
  return false;
}
bool SEPatientActionCollection::HasSubstanceCompoundInfusion(const SESubstanceCompound& cmpd) const
{
  return GetSubstanceCompoundInfusion(cmpd) != nullptr;
}
SESubstanceCompoundInfusion& SEPatientActionCollection::GetSubstanceCompoundInfusion(const SESubstanceCompound& cmpd)
{
  for (auto sci : m_SubstanceCompoundInfusions)
    if (&sci->GetSubstanceCompound() == &cmpd)
      return *sci;
  SESubstanceCompoundInfusion* sci = new SESubstanceCompoundInfusion(cmpd, GetLogger());
  m_SubstanceCompoundInfusions.push_back(sci);
  return *sci;
}
const SESubstanceCompoundInfusion* SEPatientActionCollection::GetSubstanceCompoundInfusion(const SESubstanceCompound& cmpd) const
{
  for (auto sci : m_SubstanceCompoundInfusions)
    if (&sci->GetSubstanceCompound() == &cmpd)
      return sci;
  return nullptr;
}
const std::vector<SESubstanceCompoundInfusion*>& SEPatientActionCollection::GetSubstanceCompoundInfusions()
{
  return m_SubstanceCompoundInfusions;
}
const std::vector<const SESubstanceCompoundInfusion*> SEPatientActionCollection::GetSubstanceCompoundInfusions() const
{

  return std::vector<const SESubstanceCompoundInfusion*>(m_SubstanceCompoundInfusions.begin(), m_SubstanceCompoundInfusions.end());
}
void SEPatientActionCollection::RemoveSubstanceCompoundInfusion(const SESubstanceCompound& sub)
{
  GetSubstanceCompoundInfusion(sub).Deactivate();
}

bool SEPatientActionCollection::HasTensionPneumothorax() const
{
  if (m_LeftOpenTensionPneumothorax != nullptr&&m_LeftOpenTensionPneumothorax->IsActive())
    return true;
  if (m_LeftClosedTensionPneumothorax != nullptr&&m_LeftClosedTensionPneumothorax->IsActive())
    return true;
  if (m_RightOpenTensionPneumothorax != nullptr&&m_RightOpenTensionPneumothorax->IsActive())
    return true;
  if (m_RightClosedTensionPneumothorax != nullptr&&m_RightClosedTensionPneumothorax->IsActive())
    return true;
  return false;
}
bool SEPatientActionCollection::HasLeftOpenTensionPneumothorax() const
{
  if (m_LeftOpenTensionPneumothorax != nullptr&&m_LeftOpenTensionPneumothorax->IsActive())
    return true;
  return false;
}
SETensionPneumothorax& SEPatientActionCollection::GetLeftOpenTensionPneumothorax()
{
  if (m_LeftOpenTensionPneumothorax == nullptr)
    m_LeftOpenTensionPneumothorax = new SETensionPneumothorax(GetLogger());
  return *m_LeftOpenTensionPneumothorax;
}
const SETensionPneumothorax* SEPatientActionCollection::GetLeftOpenTensionPneumothorax() const
{
  return m_LeftOpenTensionPneumothorax;
}
void SEPatientActionCollection::RemoveLeftOpenTensionPneumothorax()
{
  if (m_LeftOpenTensionPneumothorax)
    m_LeftOpenTensionPneumothorax->Deactivate();
}
bool SEPatientActionCollection::HasLeftClosedTensionPneumothorax() const
{
  if (m_LeftClosedTensionPneumothorax != nullptr&&m_LeftClosedTensionPneumothorax->IsActive())
    return true;
  return false;
}
SETensionPneumothorax& SEPatientActionCollection::GetLeftClosedTensionPneumothorax()
{
  if (m_LeftClosedTensionPneumothorax == nullptr)
    m_LeftClosedTensionPneumothorax = new SETensionPneumothorax(GetLogger());
  return *m_LeftClosedTensionPneumothorax;
}
const SETensionPneumothorax* SEPatientActionCollection::GetLeftClosedTensionPneumothorax() const
{
  return m_LeftClosedTensionPneumothorax;
}
void SEPatientActionCollection::RemoveLeftClosedTensionPneumothorax()
{
  if (m_LeftClosedTensionPneumothorax)
    m_LeftClosedTensionPneumothorax->Deactivate();
}
bool SEPatientActionCollection::HasRightOpenTensionPneumothorax() const
{
  if (m_RightOpenTensionPneumothorax != nullptr&&m_RightOpenTensionPneumothorax->IsActive())
    return true;
  return false;
}
SETensionPneumothorax& SEPatientActionCollection::GetRightOpenTensionPneumothorax()
{
  if (m_RightOpenTensionPneumothorax == nullptr)
    m_RightOpenTensionPneumothorax = new SETensionPneumothorax(GetLogger());
  return *m_RightOpenTensionPneumothorax;
}
const SETensionPneumothorax* SEPatientActionCollection::GetRightOpenTensionPneumothorax() const
{
  return m_RightOpenTensionPneumothorax;
}
void SEPatientActionCollection::RemoveRightOpenTensionPneumothorax()
{
  if (m_RightOpenTensionPneumothorax)
    m_RightOpenTensionPneumothorax->Deactivate();
}
bool SEPatientActionCollection::HasRightClosedTensionPneumothorax() const
{
  if (m_RightClosedTensionPneumothorax != nullptr&&m_RightClosedTensionPneumothorax->IsActive())
    return true;
  return false;
}
SETensionPneumothorax& SEPatientActionCollection::GetRightClosedTensionPneumothorax()
{
  if (m_RightClosedTensionPneumothorax == nullptr)
    m_RightClosedTensionPneumothorax = new SETensionPneumothorax(GetLogger());
  return *m_RightClosedTensionPneumothorax;
}
const SETensionPneumothorax* SEPatientActionCollection::GetRightClosedTensionPneumothorax() const
{
  return m_RightClosedTensionPneumothorax;
}
void SEPatientActionCollection::RemoveRightClosedTensionPneumothorax()
{
  if (m_RightClosedTensionPneumothorax)
    m_RightClosedTensionPneumothorax->Deactivate();
}

bool SEPatientActionCollection::HasTubeThoracostomy() const
{
  if (m_LeftTubeThoracostomy != nullptr&&m_LeftTubeThoracostomy->IsActive())
    return true;
  if (m_RightTubeThoracostomy != nullptr&&m_RightTubeThoracostomy->IsActive())
    return true;
  return false;
}
bool SEPatientActionCollection::HasLeftTubeThoracostomy() const
{
  if (m_LeftTubeThoracostomy != nullptr&&m_LeftTubeThoracostomy->IsActive())
    return true;
  return false;
}
SETubeThoracostomy& SEPatientActionCollection::GetLeftTubeThoracostomy()
{
  if (m_LeftTubeThoracostomy == nullptr)
    m_LeftTubeThoracostomy = new SETubeThoracostomy(GetLogger());
  return *m_LeftTubeThoracostomy;
}
const SETubeThoracostomy* SEPatientActionCollection::GetLeftTubeThoracostomy() const
{
  return m_LeftTubeThoracostomy;
}
void SEPatientActionCollection::RemoveLeftTubeThoracostomy()
{
  if (m_LeftTubeThoracostomy)
    m_LeftTubeThoracostomy->Deactivate();
}
bool SEPatientActionCollection::HasRightTubeThoracostomy() const
{
  if (m_RightTubeThoracostomy != nullptr&&m_RightTubeThoracostomy->IsActive())
    return true;
  return false;
}
SETubeThoracostomy& SEPatientActionCollection::GetRightTubeThoracostomy()
{
  if (m_RightTubeThoracostomy == nullptr)
    m_RightTubeThoracostomy = new SETubeThoracostomy(GetLogger());
  return *m_RightTubeThoracostomy;
}
const SETubeThoracostomy* SEPatientActionCollection::GetRightTubeThoracostomy() const
{
  return m_RightTubeThoracostomy;
}
void SEPatientActionCollection::RemoveRightTubeThoracostomy()
{
  if (m_RightTubeThoracostomy)
    m_RightTubeThoracostomy->Deactivate();
}

bool SEPatientActionCollection::HasUrinate() const
{
  return m_Urinate == nullptr ? false : m_Urinate->IsActive();
}
SEUrinate& SEPatientActionCollection::GetUrinate()
{
  if (m_Urinate == nullptr)
    m_Urinate = new SEUrinate(GetLogger());
  return *m_Urinate;
}
const SEUrinate* SEPatientActionCollection::GetUrinate() const
{
  return m_Urinate;
}
void SEPatientActionCollection::RemoveUrinate()
{
  if (m_Urinate)
    m_Urinate->Deactivate();
}

void SEPatientActionCollection::GetAllActions(std::vector<const SEAction*>& actions) const
{
  if (HasAcuteRespiratoryDistressSyndromeExacerbation())
    actions.push_back(GetAcuteRespiratoryDistressSyndromeExacerbation());
  if (HasAcuteStress())
    actions.push_back(GetAcuteStress());
  if (HasAirwayObstruction())
    actions.push_back(GetAirwayObstruction());
  if (HasArrhythmia())
    actions.push_back(GetArrhythmia());
  if (HasAsthmaAttack())
    actions.push_back(GetAsthmaAttack());
  if (HasBrainInjury())
    actions.push_back(GetBrainInjury());
  if (HasBronchoconstriction())
    actions.push_back(GetBronchoconstriction());
  if (HasCardiovascularMechanicsModification())
    actions.push_back(GetCardiovascularMechanicsModification());
  if (HasChestCompressionAutomated())
    actions.push_back(GetChestCompressionAutomated());
  if (HasChestCompressionInstantaneous())
    actions.push_back(GetChestCompressionInstantaneous());
  if (HasLeftChestOcclusiveDressing())
    actions.push_back(GetLeftChestOcclusiveDressing());
  if (HasRightChestOcclusiveDressing())
    actions.push_back(GetRightChestOcclusiveDressing());
  if (HasChronicObstructivePulmonaryDiseaseExacerbation())
    actions.push_back(GetChronicObstructivePulmonaryDiseaseExacerbation());
  if (HasConsciousRespiration())
    actions.push_back(GetConsciousRespiration());
  if (HasConsumeNutrients())
    actions.push_back(GetConsumeNutrients());
  if (HasDyspnea())
    actions.push_back(GetDyspnea());
  if (HasExercise())
    actions.push_back(GetExercise());
  for (auto a : m_Hemorrhages)
  {
    if(a->IsActive())
      actions.push_back(a);
  }
  if (HasLeftHemothorax())
    actions.push_back(GetLeftHemothorax());
  if (HasRightHemothorax())
    actions.push_back(GetRightHemothorax());
  if (HasImpairedAlveolarExchangeExacerbation())
    actions.push_back(GetImpairedAlveolarExchangeExacerbation());
  if (HasIntubation())
    actions.push_back(GetIntubation());
  if (HasMechanicalVentilation())
    actions.push_back(GetMechanicalVentilation());
  if (HasLeftNeedleDecompression())
    actions.push_back(GetLeftNeedleDecompression());
  if (HasRightNeedleDecompression())
    actions.push_back(GetRightNeedleDecompression());
  if (HasPericardialEffusion())
    actions.push_back(GetPericardialEffusion());
  if (HasPneumoniaExacerbation())
    actions.push_back(GetPneumoniaExacerbation());
  if (HasPulmonaryShuntExacerbation())
    actions.push_back(GetPulmonaryShuntExacerbation());
  if (HasRespiratoryFatigue())
    actions.push_back(GetRespiratoryFatigue());
  if (HasRespiratoryMechanicsConfiguration())
    actions.push_back(GetRespiratoryMechanicsConfiguration());
  if (HasRespiratoryMechanicsModification())
    actions.push_back(GetRespiratoryMechanicsModification());
  if (HasSupplementalOxygen())
    actions.push_back(GetSupplementalOxygen());
  if (HasLeftClosedTensionPneumothorax())
    actions.push_back(GetLeftClosedTensionPneumothorax());
  if (HasLeftOpenTensionPneumothorax())
    actions.push_back(GetLeftOpenTensionPneumothorax());
  if (HasRightClosedTensionPneumothorax())
    actions.push_back(GetRightClosedTensionPneumothorax());
  if (HasRightOpenTensionPneumothorax())
    actions.push_back(GetRightOpenTensionPneumothorax());
  if (HasLeftTubeThoracostomy())
    actions.push_back(GetLeftTubeThoracostomy());
  if (HasRightTubeThoracostomy())
    actions.push_back(GetRightTubeThoracostomy());
  for (auto a : m_SubstanceBoluses)
  {
    if (a->IsActive())
      actions.push_back(a);
  }
  for (auto a : m_SubstanceInfusions)
  {
    if (a->IsActive())
      actions.push_back(a);
  }
  for (auto a : m_SubstanceCompoundInfusions)
  {
    if (a->IsActive())
      actions.push_back(a);
  }
  if (HasUrinate())
    actions.push_back(GetUrinate());
}

const SEScalar* SEPatientActionCollection::GetScalar(const std::string& actionName, const std::string& cmptName, const std::string& substance, const std::string& property)
{
  if (actionName == "AcuteRespiratoryDistressSyndromeExacerbation")
    return GetAcuteRespiratoryDistressSyndromeExacerbation().GetScalar(property);
  if (actionName == "AcuteStress")
    return GetAcuteStress().GetScalar(property);
  if (actionName == "AirwayObstruction")
    return GetAirwayObstruction().GetScalar(property);
  if (actionName == "Arrhythmia")
    return GetArrhythmia().GetScalar(property);
  if (actionName == "AsthmaAttack")
    return GetAsthmaAttack().GetScalar(property);
  if (actionName == "BrainInjury")
    return GetBrainInjury().GetScalar(property);
  if (actionName == "Bronchoconstriction")
    return GetBronchoconstriction().GetScalar(property);
  if (actionName == "CardiovascularMechanicsModification")
    return GetCardiovascularMechanicsModification().GetScalar(property);
  if (actionName == "ChestCompressionAutomated")
    return GetChestCompressionAutomated().GetScalar(property);
  if (actionName == "ChestCompressionInstantaneous")
    return GetChestCompressionInstantaneous().GetScalar(property);
  if (actionName == "LeftChestOcclusiveDressing")
    return GetLeftChestOcclusiveDressing().GetScalar(property);
  if (actionName == "RightChestOcclusiveDressing")
    return GetRightChestOcclusiveDressing().GetScalar(property);
  if (actionName == "ChronicObstructivePulmonaryDiseaseExacerbation")
    return GetChronicObstructivePulmonaryDiseaseExacerbation().GetScalar(property);
  if (actionName == "ConsciousRespiration")
    return GetConsciousRespiration().GetScalar(property);
  if (actionName == "ConsumeNutrients")
    return GetConsumeNutrients().GetScalar(property);
  if (actionName == "Dyspnea")
    return GetDyspnea().GetScalar(property);
  if (actionName == "Exercise")
    return GetExercise().GetScalar(property);
  if (actionName == "Hemorrhage")
  {
    eHemorrhage_Compartment cmpt = eHemorrhage_Compartment_Parse(cmptName);
    if(cmpt != eHemorrhage_Compartment::None)
      return GetHemorrhage(cmpt).GetScalar(property);
    Error("Unsupported hemorrhage compartment : " + cmptName);
    return nullptr;
  }
  if (actionName == "LeftHemothorax")
    return GetLeftHemothorax().GetScalar(property);
  if (actionName == "RightHemothorax")
    return GetRightHemothorax().GetScalar(property);
  if (actionName == "ImpairedAlveolarExchangeExacerbation")
    return GetImpairedAlveolarExchangeExacerbation().GetScalar(property);
  if (actionName == "Intubation")
    return GetIntubation().GetScalar(property);
  if (actionName == "MechanicalVentilation")
    return GetMechanicalVentilation().GetScalar(property);
  if (actionName == "LeftNeedleDecompression")
    return GetLeftNeedleDecompression().GetScalar(property);
  if (actionName == "RightNeedleDecompression")
    return GetRightNeedleDecompression().GetScalar(property);
  if (actionName == "PericardialEffusion")
    return GetPericardialEffusion().GetScalar(property);
  if (actionName == "PneumoniaExacerbation")
    return GetPneumoniaExacerbation().GetScalar(property);
  if (actionName == "PulmonaryShuntExacerbation")
    return GetPulmonaryShuntExacerbation().GetScalar(property);
  if (actionName == "RespiratoryFatigue")
    return GetRespiratoryFatigue().GetScalar(property);
  if (actionName == "RespiratoryMechanicsConfiguration")
    return GetRespiratoryMechanicsConfiguration().GetScalar(property);
  if (actionName == "RespiratoryMechanicsModification")
    return GetRespiratoryMechanicsModification().GetScalar(property);
  if (actionName == "SupplementalOxygen")
    return GetSupplementalOxygen().GetScalar(property);
  if (actionName == "LeftClosedTensionPneumothorax")
    return GetLeftClosedTensionPneumothorax().GetScalar(property);
  if (actionName == "LeftOpenTensionPneumothorax")
    return GetLeftOpenTensionPneumothorax().GetScalar(property);
  if (actionName == "RightClosedTensionPneumothorax")
    return GetRightClosedTensionPneumothorax().GetScalar(property);
  if (actionName == "RightOpenTensionPneumothorax")
    return GetRightOpenTensionPneumothorax().GetScalar(property);
  if (actionName == "LeftTubeThoracostomy")
    return GetLeftTubeThoracostomy().GetScalar(property);
  if (actionName == "RightTubeThoracostomy")
    return GetRightTubeThoracostomy().GetScalar(property);
  if (actionName == "SubstanceBolus")
  {
    SESubstance* sub = m_SubMgr.GetSubstance(substance);
    if (sub == nullptr)
    {
      Error("Unknown substance : " + substance);
      return nullptr;
    }
    return GetSubstanceBolus(*sub).GetScalar(property);
  }
  if (actionName == "SubstanceInfusion")
  {
    SESubstance* sub = m_SubMgr.GetSubstance(substance);
    if (sub == nullptr)
    {
      Error("Unknown substance : " + substance);
      return nullptr;
    }
    return GetSubstanceInfusion(*sub).GetScalar(property);
  }
  if (actionName == "SubstanceCompoundInfusion")
  {
    SESubstanceCompound* cmpd = m_SubMgr.GetCompound(substance);
    if (cmpd == nullptr)
    {
      Error("Unknown substance compound : " + substance);
      return nullptr;
    }
    return GetSubstanceCompoundInfusion(*cmpd).GetScalar(property);
  }
  if (actionName == "Urinate")
    return GetUrinate().GetScalar(property);

  return nullptr;
}
