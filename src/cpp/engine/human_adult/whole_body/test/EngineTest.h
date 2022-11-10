/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/utils/testing/SETestManager.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "engine/human_adult/whole_body/controller/Controller.h"

class SESubstance;
class SETestSuite;
class HowToTracker;

namespace pulse { namespace human_adult_whole_body
{
  class EngineTest : public SETestManager
  {
    public:
      EngineTest(Logger* logger = nullptr);
      virtual ~EngineTest();

      virtual bool RunTest(const std::string& testName, const std::string& sOutputDirectory) override;

    protected:
      virtual void FillFunctionMap() override;
      typedef void(EngineTest::* testFunction)(const std::string&);
      std::map<std::string, testFunction> testMap;
      std::stringstream m_ss;


    //////////////////////////////////////////////////////////
    // Circuit Hemodynamics and Compartment Transport Tests //
    // System Specific Functions Tests as well              //
    //////////////////////////////////////////////////////////
  public:
    ////////////////////
    // Cardiovascular //
    ////////////////////
    void CardiovascularCircuitAndTransportTest(const std::string& sTestDirectory);
    void CardiovascularAndRenalCircuitAndTransportTest(const std::string& sTestDirectory);
    void CardiovascularAndTissueCircuitAndTransportTest(const std::string& sTestDirectory);
    void CardiovascularAndCerebrospinalFluidCircuitAndTransportTest(const std::string& sTestDirectory);
    void FullCardiovascularCircuitAndTransportTest(const std::string& sTestDirectory);
    void CardiovascularBloodGasesTest(const std::string& sTestDirectory);// Characterize the cv circuit
    void TuneCardiovascularCircuitTest(const std::string& sTestDirectory);
    // Cardiovascular Utilities //
    void CardiovascularCircuitScaleTests(const std::string& sTestDirectory);
  protected:
    enum CardiovascularDriver { Sinusoid = 0, Heart };
    void SinusoidHeartDriver(double time_s, double heartRate_s, double& lHeartElastance, double& rHeartElastance);
    void CardiovascularCircuitAndTransportTest(CardiovascularDriver driverType, double complianceScale, double resistanceScale, double volumeScale, double heartRate_bpm,
      double sysRes, double sysComp, double aortaRes, double aortaComp, double rightHeartRes, double venaCavaComp, bool connectTissue, bool connectRenal, bool connectCSF,
    bool balanceBloodgases, const std::string& sTestDirectory, const std::string& sTestName, bool breakOutResults);
    void TuneCardiovascularCircuitTest(SETestSuite& testSuite, const std::string& sTestDirectory, const std::string& sTestName, SEPatient& patient);

    double m_MeanAortaPressure;
    double m_MeanCVPressure;
    double m_Systolic;
    double m_Diastolic;
    double m_Pulse;
    double m_CardiacOutput;

  public:
    ///////////
    // Renal //
    ///////////
    void RenalCircuitAndTransportTest(const std::string& sTestDirectory);
    void RenalTGFFeedbackTest(const std::string& sTestDirectory);
    void RenalTGFandUPRFeedbackTest(const std::string& sTestDirectory); 
    void RenalSecretionTest(const std::string& sTestDirectory);
    void RenalUrinateTest(const std::string& sTestDirectory);
  protected:
    enum RenalFeedback { TGF, TGFandUPR };
    void RenalFeedbackTest(RenalFeedback feedback, const std::string& sTestDirectory, const std::string& sTestName);
    enum RenalSystems { Secretion, Urinating};
    void RenalSystemTest(RenalSystems systemtest, const std::string& sTestDirectory, const std::string& sTestName);

  public:
    /////////////////
    // Respiratory //
    /////////////////
    void RespiratoryCircuitAndTransportTest(const std::string& sTestDirectory);
    void RespiratoryDriverTest(const std::string& sTestDirectory);
  protected:
    enum RespiratoryConfiguration { RespiratorySolo, AnesthesiaMachineSolo, RespiratoryWithAnesthesiaMachine, RespiratoryWithInhaler, RespiratoryWithMechanicalVentilation, MechanicalVentilatorSolo, RespiratoryWithMechanicalVentilator
    };
    void RespiratoryCircuitAndTransportTest(RespiratoryConfiguration config, const std::string& sTestDirectory);

  public:
    ////////////////////////
    // Anesthesia Machine //
    ////////////////////////
    void AnesthesiaMachineCircuitAndTransportTest(const std::string& sTestDirectory);
    void RespiratoryWithAnesthesiaMachineCircuitAndTransportTest(const std::string& sTestDirectory);
  protected:
    void AnesthesiaMachineCircuitAndTransportTest(RespiratoryConfiguration config, const std::string& sTestDirectory);

  public:
    ///////////////////////////
    // Mechanical Ventilator //
    ///////////////////////////
    void MechanicalVentilatorCircuitAndTransportTest(const std::string& sTestDirectory);
    void RespiratoryWithMechanicalVentilatorCircuitAndTransportTest(const std::string& sTestDirectory);
  protected:
    void MechanicalVentilatorCircuitAndTransportTest(RespiratoryConfiguration config, const std::string& sTestDirectory);

  public:
    /////////////
    // Inhaler //
    /////////////
    void RespiratoryWithInhalerCircuitAndTransportTest(const std::string& sTestDirectory);
  protected:

  public:
    ////////////////////////////
    // Mechanical Ventilation //
    ////////////////////////////
    void RespiratoryWithMechanicalVentilationCircuitAndTransportTest(const std::string& sTestDirectory);
  protected:

  public:
    ////////////////////////////
    // Environment and Energy //
    void InternalTemperatureVariableBMRCircuitTest(const std::string& sTestDirectory);
    void InternalTemperatureVariableSkinCircuitTest(const std::string& sTestDirectory);
    void InternalTemperatureVariableCoreCircuitTest(const std::string& sTestDirectory);
    void EnvironmentVariableTemperatureCircuitTest(const std::string& sTestDirectory);
    void CombinedInternalAndEnvironmentVariableBMRandTemperatureCircuitTest(const std::string& sTestDirectory);
    void CombinedInternalAndEnvironmentSkinTempDropCircuitTest(const std::string& sTestDirectory);
    void EnvironmentISO7730ComparisonTest(const std::string& sTestDirectory);
  protected:

  public:
    //////////////////
    // Tissue Tests //
    //////////////////
    void DiffusionClearanceExcretionTests(const std::string& rptDirectory);
    void AlveolarCarbonDioxideDiffusionTest(const std::string& rptDirectory);
    void AlveolarOxygenDiffusionTest(const std::string& rptDirectory);
    void InstantPlusSimpleDiffusionTest(const std::string& rptDirectory);
    void SimpleDiffusionTwoCompartmentTest(const std::string& rptDirectory);
    void SimpleDiffusionFourCompartmentTest(const std::string& rptDirectory);
    void SimpleDiffusionHierarchyTest(const std::string& rptDirectory);
    void FacilitatedDiffusionTest(const std::string& rptDirectory);
  protected:
    void DistributeMass(SETestSuite& testSuite);
    void PerfusionLimitedDiffusionTest(SETestSuite& testSuite);
    void InstantDiffusionTest(SETestSuite& testSuite);
    void ActiveTransportTest(SETestSuite& testSuite);
    void GenericClearanceTest(SETestSuite& testSuite);
    void GenericExcretionTest(SETestSuite& testSuite);

  public:
      ///////////////////
      // Nervous Tests //
      ///////////////////
      void BrainInjuryTest(const std::string& rptDirectory);
  protected:

  public:
    ////////////////////////////
    // Acid Base Engine Tests //
    ////////////////////////////
    void FourCompartmentTestSimple(const std::string& sOutputDirectory);
    void AcidBaseFourCompartmentTest(const std::string& sOutputDirectory);
    void FiveCompartmentTestWithDiffusion(const std::string& sOutputDirectory);
    void AcidBaseFourCompartmentTestWithProductionConsumption(const std::string& sOutputDirectory);
    void AcidBaseFiveCompartmentTestWithDiffusion(const std::string& sOutputDirectory);
    void AcidBaseFiveCompartmentTestWithProductionConsumptionAndDiffusion(const std::string& sOutputDirectory);
   protected:
     void FourCompartmentTest(bool usingAcidBase, bool usingProductionConsumption, bool usingDiffusion, const std::string& sOutputDirectory);
     bool usingAcidBase;
     bool usingProductionConsumption;
     bool usingDiffusion;

  public:
     /////////////////////
     // Acid Base Tests //
     /////////////////////
    void AcidBaseMathTest(const std::string& sOutputDirectory);
    void AcidBaseFeedbackTest(const std::string& sOutputDirectory);
    void AcidBaseLimitsTest(const std::string& sOutputDirectory);
    void AcidBaseExtremeTest(const std::string & rptDirectory);
    void AcidBaseBloodGasTests(const std::string& sOutputDirectory);
  protected:
    enum bloodType { ARTERIAL, VENOUS, CAPILLARY, RESPIRATORY_ACIDOSIS, METABOLIC_ALKALOSIS, METABOLIC_ACIDOSIS, RESPIRATORY_ALKALOSIS, CUSTOM };
    void AcidBaseBloodGasTest(Controller& bg, bloodType compartment, SETestSuite& testSuite);

  public:
    ///////////////////
    // Aerosol Tests //
    ///////////////////
    void AerosolTest(const std::string& sOutputDirectory);
  protected:
    void SizeIndependentDepositionEfficencyCoefficientsTest(SETestSuite& suite, SESubstance& substance, double expectedMouthCoeff, double expectedCarinaCoeff, double expectedDeadSpaceCoeff, double expectedAlveoliCoeff);
    void DepositionFractionTest(SETestSuite& suite, SESubstance& substance, double expectedMouthDepFrac, double expectedCarinaDepFrac,
      double expectedLeftDeadSpaceDepFrac, double expectedLeftAlveoliDepFrac, double expectedRightDeadSpaceDepFrac, double expectedRightAlveoliDepFrac);

  public:
    //////////////////////////
    // General Engine Tests //
    //////////////////////////
    void ReadScenarios(const std::string& sOutputDirectory);
    void ReuseEngine(const std::string& sOutputDirectory);
    void SerializationTest(const std::string& sTestDirectory);
    void SetupPatientTest(const std::string& sTestDirectory);
  
    void ConditionCombinations(const std::string& rptDirectory);
    void SolverSpeedTest(const std::string& sTestDirectory);

  protected:
    void InhalerState(PhysiologyEngine* bg, HowToTracker& tracker);
    void InjectSuccsState(PhysiologyEngine* bg, HowToTracker& tracker, const SESubstance& succs);

    void SetupPatient(SETestSuite& testSuite, const std::string& sTestDirectory, const std::string& sTestName, SEPatient* patient, SEPatient* expectedPatient, bool expectedPass);
    bool CheckSetupPatient(SEPatient& setupPatient, SEPatient& expectedPatient, std::vector<std::string>& errs);
  public:
    //////////////////////
    // Black Box Tests //
    /////////////////////
    void EmptyBlackBoxTest(const std::string& sOutputDirectory);
    void ImposeFlowBlackBoxTest(const std::string& sOutputDirectory);
    void ImposePressureAndFlowBlackBoxTest(const std::string& sOutputDirectory);
  protected:

  public:
    ///////////////////////
    // Engine Pool Tests //
    ///////////////////////
    void AddRemoveEnginesTests(const std::string& rptDirectory);
  protected:
    void AddEngineGroupTest(SETestSuite& testSuite);
    void AddEnginesTest(SETestSuite& testSuite);
    void RemoveEnginesTest(SETestSuite& testSuite);

  };
END_NAMESPACE_EX
