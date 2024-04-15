/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "CommonDataModelTest.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceConcentration.h"
#include "cdm/substance/SESubstanceFraction.h"

#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
#include "cdm/patient/actions/SEAcuteStress.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEArrhythmia.h"
#include "cdm/patient/actions/SEAsthmaAttack.h"
#include "cdm/patient/actions/SEBrainInjury.h"
#include "cdm/patient/actions/SEBronchoconstriction.h"
#include "cdm/patient/actions/SEChestCompression.h"
#include "cdm/patient/actions/SEChestCompressionAutomated.h"
#include "cdm/patient/actions/SEChestCompressionInstantaneous.h"
#include "cdm/patient/actions/SEChestOcclusiveDressing.h"
#include "cdm/patient/actions/SEChronicObstructivePulmonaryDiseaseExacerbation.h"
#include "cdm/patient/actions/SEConsciousRespiration.h"
#include "cdm/patient/actions/SEConsumeNutrients.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SEExercise.h"
#include "cdm/patient/actions/SEForcedExhale.h"
#include "cdm/patient/actions/SEForcedInhale.h"
#include "cdm/patient/actions/SEForcedPause.h"
#include "cdm/patient/actions/SEForcedPause.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/patient/actions/SEImpairedAlveolarExchangeExacerbation.h"
#include "cdm/patient/actions/SEMechanicalVentilation.h"
#include "cdm/patient/actions/SENeedleDecompression.h"
#include "cdm/patient/actions/SEPatientAssessmentRequest.h"
#include "cdm/patient/actions/SEPneumoniaExacerbation.h"
#include "cdm/patient/actions/SEPrimaryBlastLungInjury.h"
#include "cdm/patient/actions/SEPericardialEffusion.h"
#include "cdm/patient/actions/SEPulmonaryShuntExacerbation.h"
#include "cdm/patient/actions/SERespiratoryFatigue.h"
#include "cdm/patient/actions/SERespiratoryMechanicsConfiguration.h"
#include "cdm/patient/actions/SESubstanceBolus.h"
#include "cdm/patient/actions/SESubstanceCompoundInfusion.h"
#include "cdm/patient/actions/SESubstanceInfusion.h"
#include "cdm/patient/actions/SESupplementalOxygen.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/patient/actions/SEUrinate.h"
#include "cdm/patient/actions/SEUseInhaler.h"

#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarForce.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

#include "utils/testing/SETestReport.h"
#include "utils/testing/SETestCase.h"
#include "utils/testing/SETestSuite.h"
#include "utils/FileUtils.h"
#include "utils/TimingProfile.h"

template<typename ActionType>
void TestAction(SETestCase& testCase, SESubstanceManager& subMgr, SEAction& action, const std::string& desc="")
{
  testCase.SetName(action.GetName()+desc);
  testCase.Info("Testing " + testCase.GetName());

  if (!action.IsValid())
  {
    testCase.AddFailure("Testing an invalid action configuration");
    return;
  }

  std::string s1;
  action.SerializeToString(s1, eSerializationFormat::VERBOSE_JSON);

  ActionType* a2 = static_cast<ActionType*>(SEAction::SerializeFromString(s1, eSerializationFormat::VERBOSE_JSON, subMgr));
  std::string s2;
  a2->SerializeToString(s2, eSerializationFormat::VERBOSE_JSON);

  if (s1 != s2)
  {
    testCase.AddFailure("Serialization strings do not match");
    testCase.Info("Original Action:\n"+s1);
    testCase.Info("What I read in: \n" + s2);
  }

  // Not really sure how to test this printed everything...
  // Maybe the user should pass in the number of properties they set
  // And we make sure we have that many lines + 1 (for the action name line)
  testCase.Info(action.PrettyPrint(s1));
  delete a2;
}

void CommonDataModelTest::ActionTest(const std::string& rptDirectory)
{
  std::string testName = "ActionTest";
  m_Logger->SetLogFile(rptDirectory + "/" + testName + ".log");
  m_Logger->LogToConsole(true); // Uncomment to see pretty prints

  SESubstanceManager subMgr(m_Logger);
  if (!subMgr.LoadSubstanceDirectory())
  {
    m_Logger->Fatal("Unable to load substance directory");
    return;
  }

  std::string dir = GetCurrentWorkingDirectory();
  dir.append("/substances");

  SETestReport testReport(m_Logger);
  SETestSuite& testSuite = testReport.CreateTestSuite();
  testSuite.SetName(testName);


  /////////////////////
  // Patient Actions //
  /////////////////////

  SEAcuteRespiratoryDistressSyndromeExacerbation ards;
  ards.GetSeverity(eLungCompartment::LeftLung).SetValue(0.3);
  ards.GetSeverity(eLungCompartment::RightLung).SetValue(0.2);
  TestAction<SEAcuteRespiratoryDistressSyndromeExacerbation>(testSuite.CreateTestCase(), subMgr, ards, "-Severity--LeftLungAffected-RightLungAffected");

  SEAcuteStress as;
  as.GetSeverity().SetValue(0);
  as.SetComment("Test Comment");
  TestAction<SEAcuteStress>(testSuite.CreateTestCase(), subMgr, as, "-Severity0-Comment");

  SEAirwayObstruction ao;
  ao.GetSeverity().SetValue(0.7);
  TestAction<SEAirwayObstruction>(testSuite.CreateTestCase(), subMgr, ao, "-Severity");

  SEArrhythmia arr;
  arr.SetRhythm(eHeartRhythm::SinusTachycardia);
  TestAction<SEArrhythmia>(testSuite.CreateTestCase(), subMgr, arr, "-Rhythm");

  SEAsthmaAttack asthma;
  asthma.GetSeverity().SetValue(0.4);
  TestAction<SEAsthmaAttack>(testSuite.CreateTestCase(), subMgr, asthma, "-Severity");

  SEBrainInjury tbi;
  tbi.GetSeverity().SetValue(0.6);
  tbi.SetType(eBrainInjury_Type::LeftFocal);
  TestAction<SEBrainInjury>(testSuite.CreateTestCase(), subMgr, tbi, "-Severity-Type");

  SEBronchoconstriction broncho;
  broncho.GetSeverity().SetValue(0.9);
  TestAction<SEBronchoconstriction>(testSuite.CreateTestCase(), subMgr, broncho, "-Severity");

  SEChestCompression cc;
  cc.GetForce().SetValue(300, ForceUnit::N);
  cc.GetCompressionPeriod().SetValue(5, TimeUnit::s);
  TestAction<SEChestCompression>(testSuite.CreateTestCase(), subMgr, cc, "-Force-CompressionPeriod");
  cc.GetForce().Invalidate();
  cc.GetDepth().SetValue(4, LengthUnit::cm);
  TestAction<SEChestCompression>(testSuite.CreateTestCase(), subMgr, cc, "-Depth-CompressionPeriod");

  SEChestCompressionAutomated ccAuto;
  ccAuto.GetAppliedForceFraction().SetValue(0.25);
  ccAuto.GetCompressionFrequency().SetValue(20, FrequencyUnit::Per_min);
  TestAction<SEChestCompressionAutomated>(testSuite.CreateTestCase(), subMgr, ccAuto, "-AppliedForceFraction-CompressionFrequency");
  ccAuto.GetCompressionFrequency().Invalidate();
  ccAuto.GetForce().SetValue(400, ForceUnit::N);
  TestAction<SEChestCompressionAutomated>(testSuite.CreateTestCase(), subMgr, ccAuto, "-AppliedForceFraction-Force");
  ccAuto.GetForce().Invalidate();
  ccAuto.GetDepth().SetValue(2, LengthUnit::cm);
  TestAction<SEChestCompressionAutomated>(testSuite.CreateTestCase(), subMgr, ccAuto, "-AppliedForceFraction-Depth");

  SEChestCompressionInstantaneous ccInstant;
  ccInstant.GetForce().SetValue(250, ForceUnit::N);
  TestAction<SEChestCompressionInstantaneous>(testSuite.CreateTestCase(), subMgr, ccInstant, "-Force");
  ccInstant.GetForce().Invalidate();
  ccInstant.GetDepth().SetValue(4.5, LengthUnit::cm);
  TestAction<SEChestCompressionInstantaneous>(testSuite.CreateTestCase(), subMgr, ccInstant, "-Depth");

  SEChestOcclusiveDressing coDressing;
  coDressing.SetSide(eSide::Left);
  coDressing.SetState(eSwitch::On);
  TestAction<SEChestOcclusiveDressing>(testSuite.CreateTestCase(), subMgr, coDressing, "-Side-State");

  SEChronicObstructivePulmonaryDiseaseExacerbation copd;
  copd.GetBronchitisSeverity().SetValue(0.4);
  TestAction<SEChronicObstructivePulmonaryDiseaseExacerbation>(testSuite.CreateTestCase(), subMgr, copd, "-BronchitisSeverity");
  copd.GetBronchitisSeverity().Invalidate();
  copd.GetEmphysemaSeverity(eLungCompartment::LeftLung).SetValue(0.3);
  copd.GetEmphysemaSeverity(eLungCompartment::RightLung).SetValue(0.3);
  TestAction<SEChronicObstructivePulmonaryDiseaseExacerbation>(testSuite.CreateTestCase(), subMgr, copd, "-EmphysemaSeverity");

  SEConsciousRespiration cr;
  cr.SetStartImmediately(true);
  SEForcedInhale& fi = cr.AddForcedInhale();
  fi.GetInspiratoryCapacityFraction().SetValue(0.3);
  fi.GetInhalePeriod().SetValue(6, TimeUnit::s);
  fi.GetHoldPeriod().SetValue(2, TimeUnit::s);
  fi.GetReleasePeriod().SetValue(4, TimeUnit::s);
  SEForcedExhale& fe = cr.AddForcedExhale();
  fe.GetExpiratoryReserveVolumeFraction().SetValue(0.25);
  fe.GetExhalePeriod().SetValue(5, TimeUnit::s);
  fe.GetHoldPeriod().SetValue(3, TimeUnit::s);
  fe.GetReleasePeriod().SetValue(2, TimeUnit::s);
  SEUseInhaler& ui = cr.AddUseInhaler();
  ui.SetComment("Test Comment (use inhaler)");
  SEForcedPause& fp = cr.AddForcedPause();
  fp.GetPeriod().SetValue(3, TimeUnit::s);
  TestAction<SEConsciousRespiration>(testSuite.CreateTestCase(), subMgr, cr, "-StartImmediately-Inhale-Exhale-Inhaler-Pause");

  SEConsumeNutrients cn;
  cn.SetNutritionFile("TestFile.json");
  TestAction<SEConsumeNutrients>(testSuite.CreateTestCase(), subMgr, cn, "-NutritionFile");
  cn.Clear();
  cn.GetNutrition().GetWater().SetValue(50, VolumeUnit::mL);
  TestAction<SEConsumeNutrients>(testSuite.CreateTestCase(), subMgr, cn, "-Water");

  SEDyspnea d;
  d.GetTidalVolumeSeverity().SetValue(0.1);
  TestAction<SEDyspnea>(testSuite.CreateTestCase(), subMgr, d, "-TidalVolumeSeverity");

  SEExercise e;
  e.GetIntensity().SetValue(0.75);
  TestAction<SEExercise>(testSuite.CreateTestCase(), subMgr, e, "-Intensity");

  SEHemorrhage h;
  h.GetSeverity().SetValue(0.8);
  h.SetType(eHemorrhage_Type::External);
  h.SetCompartment(eHemorrhage_Compartment::RightLeg);
  TestAction<SEHemorrhage>(testSuite.CreateTestCase(), subMgr, h, "-Severity-Type-Compartment");
  h.GetSeverity().Invalidate();
  h.SetType(eHemorrhage_Type::Internal);
  h.GetFlowRate().SetValue(1, VolumePerTimeUnit::mL_Per_s);
  h.GetTotalBloodLost().SetValue(25, VolumeUnit::mL);
  TestAction<SEHemorrhage>(testSuite.CreateTestCase(), subMgr, h, "-FlowRate-Type-Compartment-TotalBloodLost");

  SEImpairedAlveolarExchangeExacerbation imae;
  imae.GetSeverity().SetValue(0.2);
  TestAction<SEImpairedAlveolarExchangeExacerbation>(testSuite.CreateTestCase(), subMgr, imae, "-Severity");
  imae.GetSeverity().Invalidate();
  imae.GetImpairedSurfaceArea().SetValue(7, AreaUnit::cm2);
  TestAction<SEImpairedAlveolarExchangeExacerbation>(testSuite.CreateTestCase(), subMgr, imae, "-ImpairedSurfaceArea");
  imae.GetImpairedSurfaceArea().Invalidate();
  imae.GetImpairedFraction().SetValue(0.1);
  TestAction<SEImpairedAlveolarExchangeExacerbation>(testSuite.CreateTestCase(), subMgr, imae, "-ImpairedFraction");

  SEIntubation intub;
  intub.GetSeverity().SetValue(0.5);
  intub.GetAirwayResistance().SetValue(50, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  intub.SetType(eIntubation_Type::Esophageal);
  TestAction<SEIntubation>(testSuite.CreateTestCase(), subMgr, intub, "-Severity-AirwayResistance-Type");

  SEPneumoniaExacerbation lpe;
  lpe.GetSeverity(eLungCompartment::LeftLung).SetValue(0.3);
  lpe.GetSeverity(eLungCompartment::RightLung).SetValue(0.3);
  TestAction<SEPneumoniaExacerbation>(testSuite.CreateTestCase(), subMgr, lpe, "-Severity-LeftLungAffected-RightLungAffected");

  //Aaron -do this
  //SEPrimaryBlastLungInjury

  SEMechanicalVentilation mv;
  mv.SetState(eSwitch::On);
  mv.GetFlow().SetValue(5, VolumePerTimeUnit::mL_Per_min);
  mv.GetPressure().SetValue(4, PressureUnit::cmH2O);
  SESubstanceFraction& O2frac = mv.GetGasFraction(*subMgr.GetSubstance("Oxygen"));
  SESubstanceFraction& CO2frac = mv.GetGasFraction(*subMgr.GetSubstance("CarbonDioxide"));
  SESubstanceFraction& N2frac = mv.GetGasFraction(*subMgr.GetSubstance("Nitrogen"));
  O2frac.GetFractionAmount().SetValue(0.21);
  CO2frac.GetFractionAmount().SetValue(4.0E-4);
  N2frac.GetFractionAmount().SetValue(0.7896);
  mv.GetAerosol(*subMgr.GetSubstance("Albuterol")).GetConcentration().SetValue(2500, MassPerVolumeUnit::mg_Per_m3);
  TestAction<SEMechanicalVentilation>(testSuite.CreateTestCase(), subMgr, mv, "-State-Flow-Pressure-GasFraction(O2, CO2, N2)-Aerosol(O2)");

  SENeedleDecompression nd;
  nd.SetState(eSwitch::On);
  nd.SetSide(eSide::Right);
  TestAction<SENeedleDecompression>(testSuite.CreateTestCase(), subMgr, nd, "-Side-State");

  SEPatientAssessmentRequest par;
  par.SetType(ePatientAssessment_Type::CompleteBloodCount);
  TestAction<SEPatientAssessmentRequest>(testSuite.CreateTestCase(), subMgr, par, "-Type");

  SEPericardialEffusion pe;
  pe.GetEffusionRate().SetValue(0.3, VolumePerTimeUnit::mL_Per_min);
  TestAction<SEPericardialEffusion>(testSuite.CreateTestCase(), subMgr, pe, "-EffusionRate");

  SEPulmonaryShuntExacerbation pse;
  pse.GetSeverity().SetValue(0.7);
  TestAction<SEPulmonaryShuntExacerbation>(testSuite.CreateTestCase(), subMgr, pse, "-Severity");

  SERespiratoryFatigue rf;
  rf.GetSeverity().SetValue(0.2);
  TestAction<SERespiratoryFatigue>(testSuite.CreateTestCase(), subMgr, rf, "-Severity");

  SERespiratoryMechanicsConfiguration rmc;
  rmc.GetSettings();
  rmc.SetAppliedRespiratoryCycle(eAppliedRespiratoryCycle::Instantaneous);
  rmc.SetMergeType(eMergeType::Replace);
  TestAction<SERespiratoryMechanicsConfiguration>(testSuite.CreateTestCase(), subMgr, rmc, "-Settings-AppliedRepiratoryCycle-MergeType");
  rmc.Clear();
  rmc.SetSettingsFile("TestSettingsFile.json");
  TestAction<SERespiratoryMechanicsConfiguration>(testSuite.CreateTestCase(), subMgr, rmc, "-SettingsFile");

  SESubstanceBolus sb(*subMgr.GetSubstance("Morphine"));
  sb.GetAdminDuration().SetValue(2, TimeUnit::hr);
  sb.SetAdminRoute(eSubstanceAdministration_Route::Intravenous);
  sb.GetConcentration().SetValue(.3, MassPerVolumeUnit::ug_Per_mL);
  sb.GetDose().SetValue(10, VolumeUnit::mL);
  sb.GetTotalInfusedDose().SetValue(3, VolumeUnit::mL);
  TestAction<SESubstanceBolus>(testSuite.CreateTestCase(), subMgr, sb, "-AdminDuration-AdminRoute-Concentration-Dose-TotalInfusedDose");

  SESubstanceCompoundInfusion sci(*subMgr.GetCompound("Saline"));
  sci.GetBagVolume().SetValue(15, VolumeUnit::mL);
  sci.GetRate().SetValue(.25, VolumePerTimeUnit::mL_Per_min);
  TestAction<SESubstanceCompoundInfusion>(testSuite.CreateTestCase(), subMgr, sci, "-BagVolume-Rate");

  SESubstanceInfusion si(*subMgr.GetSubstance("Insulin"));
  si.GetConcentration().SetValue(.8, MassPerVolumeUnit::ug_Per_mL);
  si.GetRate().SetValue(.2, VolumePerTimeUnit::mL_Per_s);
  si.GetVolume().SetValue(4, VolumeUnit::mL);
  TestAction<SESubstanceInfusion>(testSuite.CreateTestCase(), subMgr, si, "-Concentration-Rate-Volume");

  SESupplementalOxygen so;
  so.SetDevice(eSupplementalOxygen_Device::NonRebreatherMask);
  so.GetFlow().SetValue(.25, VolumePerTimeUnit::L_Per_min);
  so.GetVolume().SetValue(25, VolumeUnit::L);
  TestAction<SESupplementalOxygen>(testSuite.CreateTestCase(), subMgr, so, "-Device-Flow-Volume");

  SETensionPneumothorax tp;
  tp.SetType(eGate::Open);
  tp.SetSide(eSide::Right);
  tp.GetSeverity().SetValue(0.4);
  TestAction<SETensionPneumothorax>(testSuite.CreateTestCase(), subMgr, tp, "-Type-Side-Severity");

  SEUrinate u;
  TestAction<SEUrinate>(testSuite.CreateTestCase(), subMgr, u, "");

  testReport.SerializeToFile(rptDirectory + "/" + testName + "Report.json");
}
