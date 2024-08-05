/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "MVGenerator.h"
#include "cdm/io/protobuf/PBUtils.h"

namespace pulse::study::multiplex_ventilation
{
  MVGenerator::MVGenerator(const std::string& logfileName, const std::string& dataDir) : Loggable(logfileName)
  {
    m_DataDir = dataDir;
    m_PatientList = nullptr;
    m_CompletedPatientList = nullptr;
  }
  MVGenerator::~MVGenerator()
  {
    SAFE_DELETE(m_PatientList);
    SAFE_DELETE(m_CompletedPatientList);
  }

  void MVGenerator::GeneratePatientList()
  {
    int id = 0;
    SAFE_DELETE(m_PatientList);
    m_PatientList = new pulse::study::bind::multiplex_ventilation::PatientStateListData();

    //unsigned int totalIterations = (((m_MaxCompliance_mL_Per_cmH2O - m_MinCompliance_mL_Per_cmH2O) / m_StepCompliance_mL_Per_cmH2O) + 1) *
    //                                ((m_MaxPEEP_cmH2O - m_MinPEEP_cmH2O) / m_StepPEEP_cmH2O + 1) *
    //                                (rint((m_MaxImpairment - m_MinImpairment) / m_StepImpairment + 1.0));

    SEScalarPressureTimePerVolume r;
    r.SetValue(MVGenerator::DefaultResistance_cmH2O_s_Per_L(), PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double m_Resistance_cmH2O_s_Per_mL = r.GetValue(PressureTimePerVolumeUnit::cmH2O_s_Per_mL);

    for (int compliance_mL_Per_cmH2O = m_MinCompliance_mL_Per_cmH2O; compliance_mL_Per_cmH2O <= m_MaxCompliance_mL_Per_cmH2O; compliance_mL_Per_cmH2O += m_StepCompliance_mL_Per_cmH2O)
    {
      for (int PEEP_cmH2O = m_MinPEEP_cmH2O; PEEP_cmH2O <= m_MaxPEEP_cmH2O; PEEP_cmH2O += m_StepPEEP_cmH2O)
      {
        // RC circuit charging equation
        // Assume tube resistances are negligable
        double targetTidalVolume_mL = 6.0 * 75.3;  /// \todo What's the best way to get the ideal body weight from the patient?
        double breathPeriod_s = 60.0 / MVGenerator::DefaultRespirationRate_Per_Min();
        double inspiratoryPeriod_s = MVGenerator::DefaultIERatio() * breathPeriod_s / (1.f + MVGenerator::DefaultIERatio());

        int PIP_cmH2O = int(targetTidalVolume_mL / (compliance_mL_Per_cmH2O * (1.0 - exp(-inspiratoryPeriod_s / (m_Resistance_cmH2O_s_Per_mL * compliance_mL_Per_cmH2O)))) + PEEP_cmH2O);

        for (double currentImpairment = m_MinImpairment; currentImpairment <= m_MaxImpairment; currentImpairment += m_StepImpairment)
        {
          auto patientData = m_PatientList->add_patients();
          patientData->set_id(id++);
          patientData->set_compliance_ml_per_cmh2o(compliance_mL_Per_cmH2O);
          patientData->set_resistance_cmh2o_s_per_l(MVGenerator::DefaultResistance_cmH2O_s_Per_L());
          patientData->set_impairmentfraction(currentImpairment);
          patientData->set_respirationrate_per_min(MVGenerator::DefaultRespirationRate_Per_Min());
          patientData->set_ieratio(MVGenerator::DefaultIERatio());
          patientData->set_peep_cmh2o(PEEP_cmH2O);
          patientData->set_pip_cmh2o(PIP_cmH2O);
        }
      }
    }
  }

  bool MVGenerator::Run(const std::string& stateDir, const std::string listFilename)
  {
    TimingProfile profiler;
    profiler.Start("Total");
    m_PatientStateListFile = listFilename;

    GeneratePatientList();
    // Get the ID's of Patients we need to run
    m_PatientsToRun.clear();
    for (int i = 0; i < m_PatientList->patients_size(); i++)
      m_PatientsToRun.insert(m_PatientList->patients()[i].id());

    // Remove any id's we have in the results
    SAFE_DELETE(m_CompletedPatientList);
    m_CompletedPatientList = new pulse::study::bind::multiplex_ventilation::PatientStateListData();
    if (FileExists(listFilename))
    {
      if (!SerializeFromFile(listFilename))
        return false;
      if (m_CompletedPatientList->patients_size() > 0)
      {
        Info("Found " + std::to_string(m_CompletedPatientList->patients_size()) + " previously run Patients");
        for (int i = 0; i < m_CompletedPatientList->patients_size(); i++)
          m_PatientsToRun.erase(m_CompletedPatientList->patients()[i].id());
      }
    }

    size_t numPatientsToRun = m_PatientList->patients_size() - m_CompletedPatientList->patients_size();
    if (numPatientsToRun == 0)
    {
      Info("All Patients are run in the results file");
      return true;
    }
    size_t processor_count = std::thread::hardware_concurrency();
    if (processor_count == 0)
    {
      Fatal("Unable to detect number of processors");
      return false;
    }
    // Let's not kill the computer this is running on...
    if (processor_count > 1)
      processor_count -= 1;
    // Let's not kick off more threads than we need
    if (processor_count > numPatientsToRun)
      processor_count = numPatientsToRun;
    Info("Starting " + std::to_string(processor_count) + " Threads to generate " + std::to_string(numPatientsToRun) + " patients");
    // Crank up our threads
    for (size_t p = 0; p < processor_count; p++)
      m_Threads.push_back(std::thread(&MVGenerator::ControllerLoop, this));

    for (size_t p = 0; p < processor_count; p++)
      m_Threads[p].join();

    Info("It took " + cdm::to_string(profiler.GetElapsedTime_s("Total")) + "s to run this Patient list");
    profiler.Clear();
    return true;
  }

  void MVGenerator::ControllerLoop()
  {
    pulse::study::bind::multiplex_ventilation::PatientStateData* p;
    while (true)
    {
      p = GetNextPatient();
      if (p == nullptr)
        break;
      GenerateStabilizedPatient(*p, false);
      FinalizePatient(*p);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Generate a set of patients stabilized on a ventilator
  /// 1 ventilator for each patient
  //--------------------------------------------------------------------------------------------------
  bool MVGenerator::GenerateStabilizedPatient(pulse::study::bind::multiplex_ventilation::PatientStateData& pData, bool logToConsole)
  {
    // Construct our engine name
    std::string baseName = "comp=" + cdm::to_string(pData.compliance_ml_per_cmh2o()) +
      "_peep=" + cdm::to_string(pData.peep_cmh2o()) +
      "_pip=" + cdm::to_string(pData.pip_cmh2o()) +
      "_imp=" + cdm::to_string(pData.impairmentfraction());

    MakeDirectory(Dir::Solo + "/csv/");
    auto engine = CreatePulseEngine();
    engine->GetLogger()->SetLogFile(Dir::Solo + "/log/" + baseName + ".log");
    engine->SerializeFromFile("./states/StandardMale@0s.pbb");
    MVEngine::TrackData(*engine->GetEngineTracker(), Dir::Solo + "/csv/" + baseName + ".csv");
    engine->GetLogger()->LogToConsole(logToConsole);
    engine->GetLogger()->Info("Creating engine " + baseName);

    const SESubstance* Oxygen = engine->GetSubstanceManager().GetSubstance("Oxygen");

    SEDyspnea dyspnea;
    dyspnea.GetTidalVolumeSeverity().SetValue(1.0);

    SEIntubation intubation;
    intubation.SetType(eIntubation_Type::Tracheal);

    SEOverrides overrides;
    overrides.GetScalarProperties()["RespiratoryResistance"] = SEScalarPair(pData.resistance_cmh2o_s_per_l(), PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    overrides.GetScalarProperties()["RespiratoryCompliance"] = SEScalarPair(pData.compliance_ml_per_cmh2o(), VolumePerPressureUnit::mL_Per_cmH2O);

    SEImpairedAlveolarExchangeExacerbation impairedAlveolarExchange;
    impairedAlveolarExchange.GetSeverity().SetValue(pData.impairmentfraction());

    SEPulmonaryShuntExacerbation pulmonaryShunt;
    pulmonaryShunt.GetSeverity().SetValue(pData.impairmentfraction());

    // Setup the PC-CMV ventilator
    SEMechanicalVentilatorConfiguration mvc(engine->GetLogger());
    auto& mv = mvc.GetSettings();
    mv.SetConnection(eSwitch::On);
    mv.SetInspirationWaveform(eDriverWaveform::Square);
    mv.SetExpirationWaveform(eDriverWaveform::Square);
    mv.GetPeakInspiratoryPressure().SetValue(pData.pip_cmh2o(), PressureUnit::cmH2O);
    mv.GetPositiveEndExpiratoryPressure().SetValue(pData.peep_cmh2o(), PressureUnit::cmH2O);
    double respirationRate_per_min = pData.respirationrate_per_min();
    double IERatio = pData.ieratio();

    // Translate ventilator settings
    double totalPeriod_s = 60.0 / respirationRate_per_min;
    double inspiratoryPeriod_s = IERatio * totalPeriod_s / (1 + IERatio);
    double expiratoryPeriod_s = totalPeriod_s - inspiratoryPeriod_s;
    mv.GetInspirationMachineTriggerTime().SetValue(expiratoryPeriod_s, TimeUnit::s);
    mv.GetExpirationCycleTime().SetValue(inspiratoryPeriod_s, TimeUnit::s);

    // Start at Atmospheric FiO2
    SESubstanceFraction* FiO2 = &mv.GetFractionInspiredGas(*Oxygen);
    FiO2->GetFractionAmount().SetValue(0.21);

    // Add our initial actions
    engine->ProcessAction(dyspnea);
    engine->ProcessAction(intubation);
    engine->ProcessAction(overrides);
    engine->ProcessAction(mvc);
    engine->ProcessAction(impairedAlveolarExchange);
    engine->ProcessAction(pulmonaryShunt);
    StabilizeSpO2(*engine);

    float  targetSpO2 = 0.92f;
    double previousFiO2 = FiO2->GetFractionAmount().GetValue();
    double previousSpO2 = engine->GetBloodChemistrySystem()->GetOxygenSaturation();
    double currentFiO2 = previousFiO2;
    double currentSpO2 = previousSpO2;
    engine->GetLogger()->Info("Starting stabilization at " + cdm::to_string(previousSpO2) + " SpO2");

    while (true)
    {
      StabilizeSpO2(*engine);
      currentSpO2 = engine->GetBloodChemistrySystem()->GetOxygenSaturation();
      if (currentSpO2 >= targetSpO2)
        break;

      if (currentSpO2 < previousSpO2)// Going down...
      {
        if (currentSpO2 < 0.85)
          currentFiO2 += 0.1;
        else
          currentFiO2 += 0.025;
      }
      else
      {
        if (currentSpO2 < 0.88)
          currentFiO2 += 0.05;
        else
          currentFiO2 += 0.01;
      }
      if (currentFiO2 > 0.9995)
      {
        currentFiO2 = 0.9995;
        break;
      }

      if (currentFiO2 > 0.5)
        targetSpO2 = 0.89f;

      engine->GetLogger()->Info("Setting FiO2 to " + cdm::to_string(currentFiO2) + " with an SpO2 of " + cdm::to_string(currentSpO2));
      FiO2->GetFractionAmount().SetValue(currentFiO2);
      engine->ProcessAction(mvc);
      previousSpO2 = currentSpO2;
      previousFiO2 = currentFiO2;
    }
    engine->GetLogger()->Info("Engine stabilized at an SpO2 of " + cdm::to_string(currentSpO2) + "; FiO2 of " + cdm::to_string(currentFiO2));

    // Save our state
    baseName += "_FiO2=" + cdm::to_string(currentFiO2);
    engine->GetLogger()->Info("Saving engine state" + baseName + ".json");
    engine->SerializeToFile(Dir::Solo + baseName + ".json");
    // Append to our "list" of generated states
    pData.set_statefile(Dir::Solo + baseName + ".json");
    pData.set_fio2(FiO2->GetFractionAmount().GetValue());
    // Set Vitals
    auto AortaO2 = engine->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(*engine->GetSubstanceManager().GetSubstance("Oxygen"));
    auto AortaCO2 = engine->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(*engine->GetSubstanceManager().GetSubstance("CarbonDioxide"));

    pData.set_airwayflow_l_per_min(engine->GetRespiratorySystem()->GetInspiratoryFlow(VolumePerTimeUnit::L_Per_min));
    pData.set_airwaypressure_cmh2o(engine->GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Airway)->GetPressure(PressureUnit::cmH2O));
    pData.set_alveolararterialgradient_mmhg(engine->GetRespiratorySystem()->GetAlveolarArterialGradient(PressureUnit::mmHg));
    pData.set_arterialcarbondioxidepartialpressure_mmhg(AortaCO2->GetPartialPressure(PressureUnit::mmHg));
    pData.set_arterialoxygenpartialpressure_mmhg(AortaO2->GetPartialPressure(PressureUnit::mmHg));
    pData.set_horowitzindex_mmhg(engine->GetRespiratorySystem()->GetHorowitzIndex(PressureUnit::mmHg));
    pData.set_endtidalcarbondioxidepressure_mmhg(engine->GetRespiratorySystem()->GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg));
    pData.set_idealbodyweight_kg(engine->GetPatient().GetIdealBodyWeight(MassUnit::kg));
    pData.set_meanairwaypressure_cmh2o(engine->GetRespiratorySystem()->GetMeanAirwayPressure(PressureUnit::cmH2O));
    pData.set_oxygenationindex(engine->GetRespiratorySystem()->GetOxygenationIndex());
    pData.set_oxygensaturation(engine->GetBloodChemistrySystem()->GetOxygenSaturation());
    pData.set_oxygensaturationindex_mmhg(engine->GetRespiratorySystem()->GetOxygenSaturationIndex(PressureUnit::mmHg));
    pData.set_sfratio(engine->GetRespiratorySystem()->GetSaturationAndFractionOfInspiredOxygenRatio());
    pData.set_shuntfraction(engine->GetBloodChemistrySystem()->GetShuntFraction());
    pData.set_tidalvolume_ml(engine->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL));
    pData.set_totallungvolume_ml(engine->GetRespiratorySystem()->GetTotalLungVolume(VolumeUnit::mL));
    engine->GetLogger()->Info("#################################################################################################\n\n");
    delete engine.release();
    //RunSoloState(SoloDir+baseName+".json", SoloDir+"csv/ext/"+baseName+".json", 120);

    return true;
  }

  bool MVGenerator::StabilizeSpO2(PhysiologyEngine& eng)
  {
    // Let's shoot for with in 0.25% for 10s straight
    const SESubstance* Oxygen = eng.GetSubstanceManager().GetSubstance("Oxygen");
    auto AortaO2 = eng.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(*Oxygen);

    double currentSpO2 = 0;
    double previousSpO2 = eng.GetBloodChemistrySystem()->GetOxygenSaturation();
    double currentAortaO2 = 0;
    double previousAortaO2 = AortaO2->GetPartialPressure(PressureUnit::mmHg);

    int passes = 0;
    int totalIterations = 0;
    int passesUnder80 = 0;
    while (passes < 5)
    {
      totalIterations++;
      eng.AdvanceModelTime(2, TimeUnit::s);
      eng.GetEngineTracker()->TrackData(eng.GetSimulationTime(TimeUnit::s));
      currentSpO2 = eng.GetBloodChemistrySystem()->GetOxygenSaturation();
      if (currentSpO2 < 0.8 && currentSpO2 <= previousSpO2)
      {
        passesUnder80++;
        if (passesUnder80 > 5)
          return false;
        // Just get out of here
      }
      else
        passesUnder80 = 0;

      currentAortaO2 = AortaO2->GetPartialPressure(PressureUnit::mmHg);
      double pctDiff = GeneralMath::PercentDifference(previousAortaO2, currentAortaO2);
      if (pctDiff < 0.25)
        passes++;
      else
      {
        passes = 0;
        previousAortaO2 = currentAortaO2;
      }
    }
    eng.GetLogger()->Info("Engine stablized at an SpO2 of " + cdm::to_string(currentSpO2) + " in " + std::to_string(totalIterations * 2) + "(s)");
    return true;
  }

  pulse::study::bind::multiplex_ventilation::PatientStateData* MVGenerator::GetNextPatient()
  {
    m_mutex.lock();
    pulse::study::bind::multiplex_ventilation::PatientStateData* p = nullptr;
    if (!m_PatientsToRun.empty())
    {
      auto id = *m_PatientsToRun.begin();
      for (int i = 0; i < m_PatientList->patients_size(); i++)
      {
        p = &(*m_PatientList->mutable_patients())[i];
        if (p->id() == id)
          break;
      }
      Info("Generating Patient ID " + std::to_string(id));
      m_PatientsToRun.erase(id);
    }
    m_mutex.unlock();
    return p;
  }

  void MVGenerator::FinalizePatient(pulse::study::bind::multiplex_ventilation::PatientStateData& p)
  {
    m_mutex.lock();
    auto ps = m_CompletedPatientList->mutable_patients()->Add();
    ps->CopyFrom(p);
    SerializeToFile(*m_CompletedPatientList, m_PatientStateListFile);
    Info("Completed Patient " + std::to_string(m_CompletedPatientList->patients_size()) + " of " + std::to_string(m_PatientList->patients_size()));
    m_mutex.unlock();
  }

  bool MVGenerator::SerializeToString(pulse::study::bind::multiplex_ventilation::PatientStateListData& src, std::string& output, eSerializationFormat f) const
  {
    google::protobuf::util::JsonPrintOptions printOpts;
    printOpts.add_whitespace = true;
    printOpts.preserve_proto_field_names = true;
    printOpts.always_print_fields_with_no_presence = true;
    if (!google::protobuf::util::MessageToJsonString(src, &output, printOpts).ok())
    {
      Error("Unable to serialize Patient list");
      return false;
    }
    return true;
  }
  bool MVGenerator::SerializeToFile(pulse::study::bind::multiplex_ventilation::PatientStateListData& src, const std::string& filename) const
  {
    return PBUtils::SerializeToFile(src, filename, GetLogger());
  }
  bool MVGenerator::SerializeFromString(const std::string& src, pulse::study::bind::multiplex_ventilation::PatientStateListData& dst, eSerializationFormat f)
  {
    google::protobuf::util::JsonParseOptions parseOpts;
    auto status = google::protobuf::util::JsonStringToMessage(src, &dst, parseOpts);
    if (!status.ok())
    {
      PBUtils::LogError("MVGenerator::SerializeFromString", status.ToString(), GetLogger());
      return false;
    }
    return true;
  }
  bool MVGenerator::SerializeFromFile(const std::string& filename)
  {
    return PBUtils::SerializeFromFile(filename, *m_CompletedPatientList, GetLogger());
  }
}
