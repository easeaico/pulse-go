/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#define _USE_MATH_DEFINES

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorVolumeControl.h"
#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

#include <iomanip>

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for the Mechanical Ventilator
/// Drive respiration with equipment
///
/// \details
/// Refer to the SEMechanicalVentilator class
//--------------------------------------------------------------------------------------------------
void HowToIsoShuntAnalysis()
{
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo/HowTo_MechanicalVentilator.cpp.log");
  
  pe->GetLogger()->Info("HowTo_MechanicalVentilator");
  
 
  if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("./test_results/HowTo/HowTo_MechanicalVentilator.cpp.csv");

  std::stringstream results;

  std::vector<double> ARDSSeverityList = { 0.0, 0.3, 0.6, 0.9 };
  std::vector<double> FiO2List = { 0.21, 0.5, 1.0 };
  std::vector<double> PEEPList_cmH2O = { 5, 15, 25 };

  // Header for the table
  results << std::left
    << std::setw(30) << "ARDSSeverity"
    << std::setw(30) << "FiO2"
    << std::setw(30) << "PEEP(mmHg)"
    << std::setw(30) << "ShuntFraction"
    << std::setw(30) << "PaO2(mmHg)"
    << std::setw(30) << "FRC(L)"
    << "\n";

  // Loop through all combinations
  for (double ARDSSeverity : ARDSSeverityList) {
    for (double FiO2 : FiO2List) {
      for (double PEEP_cmH2O : PEEPList_cmH2O) {

        // Load the state to start fresh each run
        if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))// Select patient
        {
          pe->GetLogger()->Error("Could not load state, check the error");
          return;
        }

        //Dyspnea
        SEDyspnea Dyspnea;
        Dyspnea.GetTidalVolumeSeverity().SetValue(1.0);
        pe->ProcessAction(Dyspnea);

        SEIntubation intubation;
        intubation.SetType(eIntubation_Type::Tracheal);
        pe->ProcessAction(intubation);

        SEMechanicalVentilatorVolumeControl vc_ac;
        vc_ac.SetConnection(eSwitch::On);
        vc_ac.SetMode(eMechanicalVentilator_VolumeControlMode::ContinuousMandatoryVentilation);
        vc_ac.SetInspirationWaveform(eDriverWaveform::Square);
        vc_ac.GetFlow().SetValue(60.0, VolumePerTimeUnit::L_Per_min);
        vc_ac.GetFractionInspiredOxygen().SetValue(FiO2);
        vc_ac.GetInspiratoryPeriod().SetValue(1.0, TimeUnit::s); //This is optional
        vc_ac.GetPositiveEndExpiratoryPressure().SetValue(PEEP_cmH2O, PressureUnit::cmH2O);
        vc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
        vc_ac.GetTidalVolume().SetValue(500.0, VolumeUnit::mL);
        pe->ProcessAction(vc_ac);

        SEAcuteRespiratoryDistressSyndromeExacerbation ARDS;
        ARDS.GetSeverity(eLungCompartment::LeftLung).SetValue(ARDSSeverity);
        ARDS.GetSeverity(eLungCompartment::RightLung).SetValue(ARDSSeverity);
        pe->ProcessAction(ARDS);

        AdvanceAndTrackTime_s(300, *pe);

        // Log the combination
        results << std::left
          << std::setw(30) << ARDSSeverity
          << std::setw(30) << FiO2
          << std::setw(30) << PEEP_cmH2O
          << std::setw(30) << pe->GetBloodChemistrySystem()->GetShuntFraction()
          << std::setw(30) << pe->GetBloodChemistrySystem()->GetArterialOxygenPressure(PressureUnit::mmHg)
          << std::setw(30) << pe->GetPatient().GetFunctionalResidualCapacity(VolumeUnit::L)
          << "\n";

        // Output the results
        std::cout << results.str();
      }
    }
  }

  pe->GetLogger()->Info("Finished");
}
