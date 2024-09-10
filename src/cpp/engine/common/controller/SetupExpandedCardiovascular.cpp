/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/Controller.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"

#include "cdm/circuit/SECircuitManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/tissue/SETissueCompartment.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerMass.h"

namespace pulse
{
  void Controller::SetupExpandedCardiovascular()
  {
    Info("Setting Up Expanded Cardiovascular");
    bool male = m_InitialPatient->GetSex() == ePatient_Sex::Male ? true : false;
    double RightLungRatio = m_InitialPatient->GetRightLungRatio().GetValue();
    double LeftLungRatio = 1 - RightLungRatio;
    double bloodVolume_mL = m_InitialPatient->GetBloodVolumeBaseline(VolumeUnit::mL);

    double systolicPressureTarget_mmHg = m_InitialPatient->GetSystolicArterialPressureBaseline(PressureUnit::mmHg);
    double heartRate_bpm = m_InitialPatient->GetHeartRateBaseline(FrequencyUnit::Per_min);
    const double strokeVolumeTarget_mL = 81.0;
    double cardiacOutputTarget_mL_Per_s = heartRate_bpm / 60.0 * strokeVolumeTarget_mL;
    Info("Targeting Cardiac Output of " + pulse::cdm::to_string(cardiacOutputTarget_mL_Per_s) + " mL/s");
    const double pulmonaryShuntFractionFactor = 0.009; // Used to set the pulmonary shunt fraction. Actual shunt will be roughly double this value (two lungs).
    // The way this works is we compute resistances and compliances based on the hemodynamic variables above that are either in the patient
    // file or we use the defaults if nothing is there. Because the actual impedance depends on the frequency, the computations assume a resting heart rate.
    // So if a user needs to put pressures in the patient file assuming that the pts baseline hr is in the normal range (around 72).
    // If someone wants a patient with a high hr because s/he is exercising or something, then they need to use the action.
    // If a user wants a patient with a ridiculously high resting hr, then they will need to estimate what the pressures and CO would be if the pt had a normal resting hr.

    // Our configuration can apply a modifier to adjust any of these targets to get closer to the target systolic and diastolic pressures from the patient file
    // The tuning method in cardiovascular will do the fine tuning, but modifiers can speed up the process.
    /// \todo Make these a function of the systolic and diastolic pressure by fitting a curve to the data from the variations test
    const double systemicResistanceModifier = 0.6;
    const double largeArteriesComplianceModifier = 0.4333;

    //Flow rates from \cite valtin1995renal
    double VascularFlowTargetAorta = 1.0 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetArm = male ? 0.02 * cardiacOutputTarget_mL_Per_s : 0.023 * cardiacOutputTarget_mL_Per_s; //0.01448, 0.01664
    double VascularFlowTargetArmBone = 0.1 * VascularFlowTargetArm;
    double VascularFlowTargetArmFat = 0.1 * VascularFlowTargetArm;
    double VascularFlowTargetArmMicrovasculature = 0.36 * VascularFlowTargetArm;
    double VascularFlowTargetArmMuscle = 0.34 * VascularFlowTargetArm;
    double VascularFlowTargetArmSkin = 0.1 * VascularFlowTargetArm;
    double VascularFlowTargetExtracranial = 0.05 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetIntracranial = 0.114 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetKidney = male ? 0.095 * cardiacOutputTarget_mL_Per_s : 0.085 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetLargeIntestine = male ? 0.04 * cardiacOutputTarget_mL_Per_s : 0.05 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetLeg = male ? 0.0625 * cardiacOutputTarget_mL_Per_s : 0.0718 * cardiacOutputTarget_mL_Per_s; //0.02872, 0.0330
    double VascularFlowTargetLegBone = 0.1 * VascularFlowTargetLeg;
    double VascularFlowTargetLegFat = 0.1 * VascularFlowTargetLeg;
    double VascularFlowTargetLegMicrovasculature = 0.36 * VascularFlowTargetLeg;
    double VascularFlowTargetLegMuscle = 0.34 * VascularFlowTargetLeg;
    double VascularFlowTargetLegSkin = 0.1 * VascularFlowTargetLeg;
    double VascularFlowTargetLiver = 0.25 * cardiacOutputTarget_mL_Per_s; //0.075
    double VascularFlowTargetMyocardium = male ? 0.04 * cardiacOutputTarget_mL_Per_s : 0.05 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetPulmCapRight = RightLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetPulmVeinsRight = RightLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetPulmArtLeft = LeftLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetPulmCapLeft = LeftLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetPulmVeinsLeft = LeftLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetSmallIntestine = male ? 0.1 * cardiacOutputTarget_mL_Per_s : 0.11 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetSplanchnic = male ? 0.025 * cardiacOutputTarget_mL_Per_s : 0.0247 * cardiacOutputTarget_mL_Per_s; //0.0258, 0.0255
    double VascularFlowTargetSpleen = 0.03 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetTorsoBone = 0.0235 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetTorsoFat = male ? 0.0235 * cardiacOutputTarget_mL_Per_s : 0.05695 * cardiacOutputTarget_mL_Per_s; //0.05, 0.085
    double VascularFlowTargetTorsoMicrovasculature = 0.01 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetTorsoMuscle = male ? 0.1000 * cardiacOutputTarget_mL_Per_s : 0.0804 * cardiacOutputTarget_mL_Per_s; //0.17, 0.12
    double VascularFlowTargetTorsoSkin = 0.0235 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetPortalVein = VascularFlowTargetLargeIntestine + VascularFlowTargetSmallIntestine + VascularFlowTargetSplanchnic + VascularFlowTargetSpleen;

    // For male there was a total of 103.34 of used CO. Reduced flow to torso



    // Pressure targets derived from information available in \cite guyton2006medical and \cite van2013davis
    double VascularPressureTargetAorta = 1.0 * systolicPressureTarget_mmHg;
    double VascularPressureTargetArterioles = 0.75 * systolicPressureTarget_mmHg;
    double VascularPressureTargetCapillaries = 0.20 * systolicPressureTarget_mmHg;   //Capillary around 10 - 22 mmHg
    double VascularPressureTargetVenules = 0.08 * systolicPressureTarget_mmHg;   //Venules around 13 - 10 mmHg


    //double VascularPressureTargetSkin                   = 0.0833  * systolicPressureTarget_mmHg;
    double VascularPressureTargetSkin = 0.20 * systolicPressureTarget_mmHg;

    double VascularPressureTargetArmBone = VascularPressureTargetCapillaries;
    double VascularPressureTargetArmFat = VascularPressureTargetCapillaries;
    double VascularPressureTargetArmMicrovasculature = VascularPressureTargetCapillaries;
    double VascularPressureTargetArmMuscle = VascularPressureTargetCapillaries;
    double VascularPressureTargetArmSkin = VascularPressureTargetSkin;

    double VascularPressureTargetLegBone = VascularPressureTargetCapillaries;
    double VascularPressureTargetLegFat = VascularPressureTargetCapillaries;
    double VascularPressureTargetLegMicrovasculature = VascularPressureTargetCapillaries;
    double VascularPressureTargetLegMuscle = VascularPressureTargetCapillaries;
    double VascularPressureTargetLegSkin = VascularPressureTargetSkin;

    double VascularPressureTargetTorsoBone = VascularPressureTargetCapillaries;
    double VascularPressureTargetTorsoFat = VascularPressureTargetCapillaries;
    double VascularPressureTargetTorsoMicrovasculature = VascularPressureTargetCapillaries;
    double VascularPressureTargetTorsoMuscle = VascularPressureTargetCapillaries;
    double VascularPressureTargetTorsoSkin = VascularPressureTargetSkin;

    double VascularPressureTargetExtracranial = VascularPressureTargetCapillaries;
    double VascularPressureTargetIntracranial = 0.10 * systolicPressureTarget_mmHg;
    // double VascularPressureTargetExtracranial           = 0.08    * systolicPressureTarget_mmHg;
    // double VascularPressureTargetIntracranial           = 0.08    * systolicPressureTarget_mmHg;
    double VascularPressureTargetKidney = VascularPressureTargetCapillaries;

    double VascularPressureTargetMyocardium = VascularPressureTargetCapillaries;

    //double VascularPressureTargetLiver                  = 0.25    * systolicPressureTarget_mmHg;
    double VascularPressureTargetLiver = 0.15 * systolicPressureTarget_mmHg;
    double VascularPressureTargetLargeIntestine = VascularPressureTargetCapillaries;
    double VascularPressureTargetSmallIntestine = VascularPressureTargetCapillaries;
    double VascularPressureTargetSplanchnic = VascularPressureTargetCapillaries;
    double VascularPressureTargetSpleen = VascularPressureTargetCapillaries;


    double VascularPressureTargetHeartLeft = 1.06667 * systolicPressureTarget_mmHg;
    double VascularPressureTargetHeartRight = 0.16667 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmArtRight = 0.13333 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmCapRight = 0.0650 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmVeinsRight = 0.03846 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmArtLeft = 0.13333 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmCapLeft = 0.0650 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmVeinsLeft = 0.03846 * systolicPressureTarget_mmHg;

    double VascularPressureTargetVenaCava = 0.0333 * systolicPressureTarget_mmHg;

    // Volume fractions from \cite valtin1995renal
    double VolumeFractionArterioles = 0.05;
    double VolumeFractionCapillaries = 0.15;
    double VolumeFractionVenules = 0.80;

    double VolumeFraction1 = 0.15;
    double VolumeFraction2 = 0.70;

    double StressedVolume = 1;
    double UnstressedVolume = 0.7;

    double VolumeFractionAorta = 0.05;
    double VolumeFractionVenaCava = 0.09;

    double VolumeFractionExtracranial = 0.005;
    double VolumeFractionExtracranialArterioles = VolumeFractionExtracranial * VolumeFraction1;
    double VolumeFractionExtracranialCapillaries = VolumeFractionExtracranial * VolumeFraction2;
    double VolumeFractionExtracranialVenules = VolumeFractionExtracranial * VolumeFraction1;

    double VolumeFractionIntracranial = 0.012;
    double VolumeFractionIntracranialArterioles = VolumeFractionIntracranial * VolumeFraction1;
    double VolumeFractionIntracranialCapillaries = VolumeFractionIntracranial * VolumeFraction2;
    double VolumeFractionIntracranialVenules = VolumeFractionIntracranial * VolumeFraction1;

    double VolumeFractionMyocardium = 0.01;
    double VolumeFractionMyocardiumArterioles = VolumeFractionMyocardium * VolumeFraction1;
    double VolumeFractionMyocardiumCapillaries = VolumeFractionMyocardium * VolumeFraction2;
    double VolumeFractionMyocardiumVenules = VolumeFractionMyocardium * VolumeFraction1;

    double VolumeFractionKidney = 0.018;
    double VolumeFractionKidneyArterioles = VolumeFractionKidney * VolumeFraction1;
    double VolumeFractionKidneyCapillaries = VolumeFractionKidney * VolumeFraction2;
    double VolumeFractionKidneyVenules = VolumeFractionKidney * VolumeFraction1;

    // We should have around 25% of the total volume in the splanchnich circulation (\cite Catecholamine-induced Changes in the Splanchnic Circulation Affecting Systemic Hemodynamics)
    // Some blood volume has been removed from VenaCava and redistributed
    double VolumeFractionLiver = 0.1;
    double VolumeFractionLargeIntestine = 0.0202 + 0.0125;
    double VolumeFractionSmallIntestine = 0.038 + 0.0125;
    double VolumeFractionSplanchnic = 0.011 + 0.0125;
    double VolumeFractionSpleen = 0.014 + 0.0125;

    double VolumeFractionGutLiver = VolumeFractionLiver + VolumeFractionLargeIntestine + VolumeFractionSmallIntestine + VolumeFractionSplanchnic + VolumeFractionSpleen;
    double VolumeFractionGutLiverArterioles = VolumeFractionGutLiver * VolumeFraction1;
    VolumeFractionLiver = VolumeFractionLiver * VolumeFraction2;
    VolumeFractionLargeIntestine = VolumeFractionLargeIntestine * VolumeFraction2;
    VolumeFractionSmallIntestine = VolumeFractionSmallIntestine * VolumeFraction2;
    VolumeFractionSplanchnic = VolumeFractionSplanchnic * VolumeFraction2;
    VolumeFractionSpleen = VolumeFractionSpleen * VolumeFraction2;
    double VolumeFractionGutLiverVenules = VolumeFractionGutLiver * VolumeFraction1;

    double VolumeFractionHeartLeft = 0.0025;
    double VolumeFractionHeartRight = 0.0025;
    double VolumeFractionPulmArtRight = 0.034 * RightLungRatio;
    double VolumeFractionPulmCapRight = 0.023 * RightLungRatio;
    double VolumeFractionPulmVeinsRight = 0.068 * RightLungRatio;
    double VolumeFractionPulmArtLeft = 0.034 * LeftLungRatio;
    double VolumeFractionPulmCapLeft = 0.023 * LeftLungRatio;
    double VolumeFractionPulmVeinsLeft = 0.068 * LeftLungRatio;

    double VolumeFractionBone = 0.14 + 0.0825;
    double VolumeFractionFat = 0.1 + 0.05;
    double VolumeFractionMuscle = 0.28 + 0.1625;
    double VolumeFractionSkin = 0.07 + 0.02;
    double VolumeFractionMicrocirculation = 0.02 + 0.1025 + +0.1025;


    double VolumeFractionArm = 0.025;
    double VolumeFractionArmArterioles = 1 * VolumeFractionArm * VolumeFraction1;
    double VolumeFractionArmBone = 1 * VolumeFractionArm * VolumeFraction2 * VolumeFractionBone;
    double VolumeFractionArmFat = 1 * VolumeFractionArm * VolumeFraction2 * VolumeFractionFat;
    double VolumeFractionArmMicrovasculature = 1 * VolumeFractionArm * VolumeFraction2 * VolumeFractionMicrocirculation;
    double VolumeFractionArmMuscle = 1 * VolumeFractionArm * VolumeFraction2 * VolumeFractionMuscle;
    double VolumeFractionArmSkin = 1 * VolumeFractionArm * VolumeFraction2 * VolumeFractionSkin;
    double VolumeFractionArmVenules = 1 * VolumeFractionArm * VolumeFraction1;

    double VolumeFractionLeg = 0.045;
    double VolumeFractionLegArterioles = 1 * VolumeFractionLeg * VolumeFraction1;
    double VolumeFractionLegBone = 1 * VolumeFractionLeg * VolumeFraction2 * VolumeFractionBone;
    double VolumeFractionLegFat = 1 * VolumeFractionLeg * VolumeFraction2 * VolumeFractionFat;
    double VolumeFractionLegMicrovasculature = 1 * VolumeFractionLeg * VolumeFraction2 * VolumeFractionMicrocirculation;
    double VolumeFractionLegMuscle = 1 * VolumeFractionLeg * VolumeFraction2 * VolumeFractionMuscle;
    double VolumeFractionLegSkin = 1 * VolumeFractionLeg * VolumeFraction2 * VolumeFractionSkin;
    double VolumeFractionLegVenules = 1 * VolumeFractionLeg * VolumeFraction1;

    double VolumeFractionTorso = 0.213;
    double VolumeFractionTorsoArterioles = 1 * VolumeFractionTorso * VolumeFraction1;
    double VolumeFractionTorsoBone = 1 * VolumeFractionTorso * VolumeFraction2 * VolumeFractionBone;
    double VolumeFractionTorsoFat = 1 * VolumeFractionTorso * VolumeFraction2 * VolumeFractionFat;
    double VolumeFractionTorsoMicrovasculature = 1 * VolumeFractionTorso * VolumeFraction2 * VolumeFractionMicrocirculation;
    double VolumeFractionTorsoMuscle = 1 * VolumeFractionTorso * VolumeFraction2 * VolumeFractionMuscle;
    double VolumeFractionTorsoSkin = 1 * VolumeFractionTorso * VolumeFraction2 * VolumeFractionSkin;
    double VolumeFractionTorsoVenules = 1 * VolumeFractionTorso * VolumeFraction1;


    // Compute resistances from mean flow rates and pressure targets
    double ResistanceAorta = (VascularPressureTargetHeartLeft - systolicPressureTarget_mmHg) / VascularFlowTargetAorta;
    double ResistanceArterioles = (systolicPressureTarget_mmHg - VascularPressureTargetArterioles) / VascularFlowTargetAorta;
    double ResistanceArmBone = (VascularPressureTargetArterioles - VascularPressureTargetArmBone) / VascularFlowTargetArmBone;
    double ResistanceArmBoneVenous = (VascularPressureTargetArmBone - VascularPressureTargetVenules) / VascularFlowTargetArmBone;
    double ResistanceArmFat = (VascularPressureTargetArterioles - VascularPressureTargetArmFat) / VascularFlowTargetArmFat;
    double ResistanceArmFatVenous = (VascularPressureTargetArmFat - VascularPressureTargetVenules) / VascularFlowTargetArmFat;
    double ResistanceArmMicrovasculature = (VascularPressureTargetArterioles - VascularPressureTargetArmMicrovasculature) / VascularFlowTargetArmMicrovasculature;
    double ResistanceArmMicrovasculatureVenous = (VascularPressureTargetArmMicrovasculature - VascularPressureTargetVenules) / VascularFlowTargetArmMicrovasculature;
    double ResistanceArmMuscle = (VascularPressureTargetArterioles - VascularPressureTargetArmMuscle) / VascularFlowTargetArmMuscle;
    double ResistanceArmMuscleVenous = (VascularPressureTargetArmMuscle - VascularPressureTargetVenules) / VascularFlowTargetArmMuscle;
    double ResistanceArmSkin = (VascularPressureTargetArterioles - VascularPressureTargetArmSkin) / VascularFlowTargetArmSkin;
    double ResistanceArmSkinVenous = (VascularPressureTargetArmSkin - VascularPressureTargetVenules) / VascularFlowTargetArmSkin;
    double ResistanceExtracranial = (VascularPressureTargetArterioles - VascularPressureTargetExtracranial) / VascularFlowTargetExtracranial;
    double ResistanceExtracranialVenous = (VascularPressureTargetExtracranial - VascularPressureTargetVenules) / VascularFlowTargetExtracranial;
    double ResistanceIntracranial = (VascularPressureTargetArterioles - VascularPressureTargetIntracranial) / VascularFlowTargetIntracranial;
    double ResistanceIntracranialVenous = (VascularPressureTargetIntracranial - VascularPressureTargetVenules) / VascularFlowTargetIntracranial;
    double ResistanceHeartLeft = 0.0008;
    double ResistanceHeartRight = (systolicPressureTarget_mmHg - VascularPressureTargetVenules) / cardiacOutputTarget_mL_Per_s;
    double ResistanceKidney = (VascularPressureTargetArterioles - VascularPressureTargetKidney) / VascularFlowTargetKidney;
    double ResistanceKidneyVenous = (VascularPressureTargetKidney - VascularPressureTargetVenules) / VascularFlowTargetKidney;
    double ResistanceLargeIntestine = (VascularPressureTargetArterioles - VascularPressureTargetLargeIntestine) / VascularFlowTargetLargeIntestine;
    double ResistanceLargeIntestineVenous = (VascularPressureTargetLargeIntestine - VascularPressureTargetLiver) / VascularFlowTargetLargeIntestine;
    double ResistanceLegBone = (VascularPressureTargetArterioles - VascularPressureTargetLegBone) / VascularFlowTargetLegBone;
    double ResistanceLegBoneVenous = (VascularPressureTargetLegBone - VascularPressureTargetVenules) / VascularFlowTargetLegBone;
    double ResistanceLegFat = (VascularPressureTargetArterioles - VascularPressureTargetLegFat) / VascularFlowTargetLegFat;
    double ResistanceLegFatVenous = (VascularPressureTargetLegFat - VascularPressureTargetVenules) / VascularFlowTargetLegFat;
    double ResistanceLegMicrovasculature = (VascularPressureTargetArterioles - VascularPressureTargetLegMicrovasculature) / VascularFlowTargetLegMicrovasculature;
    double ResistanceLegMicrovasculatureVenous = (VascularPressureTargetLegMicrovasculature - VascularPressureTargetVenules) / VascularFlowTargetLegMicrovasculature;
    double ResistanceLegMuscle = (VascularPressureTargetArterioles - VascularPressureTargetLegMuscle) / VascularFlowTargetLegMuscle;
    double ResistanceLegMuscleVenous = (VascularPressureTargetLegMuscle - VascularPressureTargetVenules) / VascularFlowTargetLegMuscle;
    double ResistanceLegSkin = (VascularPressureTargetArterioles - VascularPressureTargetLegSkin) / VascularFlowTargetLegSkin;
    double ResistanceLegSkinVenous = (VascularPressureTargetLegSkin - VascularPressureTargetVenules) / VascularFlowTargetLegSkin;
    double ResistanceLiver = (VascularPressureTargetArterioles - VascularPressureTargetLiver) / (VascularFlowTargetLiver - VascularFlowTargetPortalVein);
    double ResistanceLiverVenous = (VascularPressureTargetLiver - VascularPressureTargetVenules) / VascularFlowTargetLiver;
    double ResistanceMyocardium = (VascularPressureTargetArterioles - VascularPressureTargetMyocardium) / VascularFlowTargetMyocardium;
    double ResistanceMyocardiumVenous = (VascularPressureTargetMyocardium - VascularPressureTargetVenules) / VascularFlowTargetMyocardium;
    double ResistancePulmCapRight = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetPulmCapRight;
    double ResistancePulmVeinsRight = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetPulmVeinsRight;
    double ResistancePulmArt = (VascularPressureTargetHeartRight - VascularPressureTargetPulmArtLeft) / VascularFlowTargetPulmArtLeft;
    double ResistancePulmCapLeft = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetPulmCapLeft;
    double ResistancePulmVeinsLeft = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetPulmVeinsLeft;
    double ResistanceSmallIntestine = (VascularPressureTargetArterioles - VascularPressureTargetSmallIntestine) / VascularFlowTargetSmallIntestine;
    double ResistanceSmallIntestineVenous = (VascularPressureTargetSmallIntestine - VascularPressureTargetLiver) / VascularFlowTargetSmallIntestine;
    double ResistanceSplanchnic = (VascularPressureTargetArterioles - VascularPressureTargetSplanchnic) / VascularFlowTargetSplanchnic;
    double ResistanceSplanchnicVenous = (VascularPressureTargetSplanchnic - VascularPressureTargetLiver) / VascularFlowTargetSplanchnic;
    double ResistanceSpleen = (VascularPressureTargetArterioles - VascularPressureTargetSpleen) / VascularFlowTargetSpleen;
    double ResistanceSpleenVenous = (VascularPressureTargetSpleen - VascularPressureTargetLiver) / VascularFlowTargetSpleen;
    double ResistanceTorsoBone = (VascularPressureTargetArterioles - VascularPressureTargetTorsoBone) / VascularFlowTargetTorsoBone;
    double ResistanceTorsoBoneVenous = (VascularPressureTargetTorsoBone - VascularPressureTargetVenules) / VascularFlowTargetTorsoBone;
    double ResistanceTorsoFat = (VascularPressureTargetArterioles - VascularPressureTargetTorsoFat) / VascularFlowTargetTorsoFat;
    double ResistanceTorsoFatVenous = (VascularPressureTargetTorsoFat - VascularPressureTargetVenules) / VascularFlowTargetTorsoFat;
    double ResistanceTorsoMicrovasculature = (VascularPressureTargetArterioles - VascularPressureTargetTorsoMicrovasculature) / VascularFlowTargetTorsoMicrovasculature;
    double ResistanceTorsoMicrovasculatureVenous = (VascularPressureTargetTorsoMicrovasculature - VascularPressureTargetVenules) / VascularFlowTargetTorsoMicrovasculature;
    double ResistanceTorsoMuscle = (VascularPressureTargetArterioles - VascularPressureTargetTorsoMuscle) / VascularFlowTargetTorsoMuscle;
    double ResistanceTorsoMuscleVenous = (VascularPressureTargetTorsoMuscle - VascularPressureTargetVenules) / VascularFlowTargetTorsoMuscle;
    double ResistanceTorsoSkin = (VascularPressureTargetArterioles - VascularPressureTargetTorsoSkin) / VascularFlowTargetTorsoSkin;
    double ResistanceTorsoSkinVenous = (VascularPressureTargetTorsoSkin - VascularPressureTargetVenules) / VascularFlowTargetTorsoSkin;

    double ResistanceVenules = (VascularPressureTargetVenules - VascularPressureTargetVenaCava) / VascularFlowTargetAorta;
    // Portal vein and shunt are just paths - only have resistance
    double ResistancePortalVein = 0.001; // The portal vein is just a pathway in Pulse. The pressure across this path does not represent portal vein pressure (if it did our patient would always be portal hypertensive)
    double ResistanceShuntRight = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / (cardiacOutputTarget_mL_Per_s * pulmonaryShuntFractionFactor);
    double ResistanceShuntLeft = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / (cardiacOutputTarget_mL_Per_s * pulmonaryShuntFractionFactor);

    // Make a circuit
    SEFluidCircuit& cCardiovascular = m_Circuits->GetCardiovascularCircuit();

    // Create Nodes, set volume baselines and pressures where appropriate
    SEFluidCircuitNode& RightHeart1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightHeart1);
    RightHeart1.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitNode& RightHeart2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightHeart2);
    SEFluidCircuitNode& RightHeart3 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightHeart3);
    RightHeart3.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    RightHeart1.GetVolumeBaseline().SetValue(VolumeFractionHeartRight * bloodVolume_mL, VolumeUnit::mL);

    SEFluidCircuitNode& MainPulmonaryArteries1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::MainPulmonaryArteries1);

    SEFluidCircuitNode& RightIntermediatePulmonaryArteries1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightIntermediatePulmonaryArteries1);
    SEFluidCircuitNode& RightPulmonaryArteries1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightPulmonaryArteries1);
    RightPulmonaryArteries1.GetVolumeBaseline().SetValue(VolumeFractionPulmArtRight * bloodVolume_mL, VolumeUnit::mL);
    RightPulmonaryArteries1.GetPressure().SetValue(VascularPressureTargetPulmArtRight, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftIntermediatePulmonaryArteries1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftIntermediatePulmonaryArteries1);
    SEFluidCircuitNode& LeftPulmonaryArteries1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftPulmonaryArteries1);
    LeftPulmonaryArteries1.GetVolumeBaseline().SetValue(VolumeFractionPulmArtLeft * bloodVolume_mL, VolumeUnit::mL);
    LeftPulmonaryArteries1.GetPressure().SetValue(VascularPressureTargetPulmArtLeft, PressureUnit::mmHg);

    SEFluidCircuitNode& RightPulmonaryCapillaries1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightPulmonaryCapillaries1);
    RightPulmonaryCapillaries1.GetVolumeBaseline().SetValue(VolumeFractionPulmCapRight * bloodVolume_mL, VolumeUnit::mL);
    RightPulmonaryCapillaries1.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftPulmonaryCapillaries1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftPulmonaryCapillaries1);
    LeftPulmonaryCapillaries1.GetVolumeBaseline().SetValue(VolumeFractionPulmCapLeft * bloodVolume_mL, VolumeUnit::mL);
    LeftPulmonaryCapillaries1.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);

    SEFluidCircuitNode& RightIntermediatePulmonaryVeins1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightIntermediatePulmonaryVeins1);
    SEFluidCircuitNode& RightPulmonaryVeins1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightPulmonaryVeins1);
    RightPulmonaryVeins1.GetVolumeBaseline().SetValue(VolumeFractionPulmVeinsRight * bloodVolume_mL, VolumeUnit::mL);
    RightPulmonaryVeins1.GetPressure().SetValue(VascularPressureTargetPulmVeinsRight, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftIntermediatePulmonaryVeins1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftIntermediatePulmonaryVeins1);
    SEFluidCircuitNode& LeftPulmonaryVeins1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftPulmonaryVeins1);
    LeftPulmonaryVeins1.GetVolumeBaseline().SetValue(VolumeFractionPulmVeinsLeft * bloodVolume_mL, VolumeUnit::mL);
    LeftPulmonaryVeins1.GetPressure().SetValue(VascularPressureTargetPulmVeinsLeft, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftHeart1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftHeart1);
    LeftHeart1.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitNode& LeftHeart2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftHeart2);
    SEFluidCircuitNode& LeftHeart3 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftHeart3);
    LeftHeart3.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    LeftHeart1.GetVolumeBaseline().SetValue(VolumeFractionHeartLeft * bloodVolume_mL, VolumeUnit::mL);

    SEFluidCircuitNode& Aorta1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Aorta1);
    SEFluidCircuitNode& Aorta2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Aorta2);
    SEFluidCircuitNode& Aorta3 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Aorta3);
    SEFluidCircuitNode& Aorta4 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Aorta4);
    SEFluidCircuitNode& Aorta5 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Aorta5);
    Aorta4.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    Aorta1.GetVolumeBaseline().SetValue(70, VolumeUnit::mL);
    Aorta1.GetPressure().SetValue(VascularPressureTargetAorta, PressureUnit::mmHg);
    Aorta5.GetVolumeBaseline().SetValue(VolumeFractionAorta * bloodVolume_mL, VolumeUnit::mL);
    Aorta5.GetPressure().SetValue(VascularPressureTargetAorta, PressureUnit::mmHg);

    SEFluidCircuitNode& Extracranial1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Extracranial1);
    SEFluidCircuitNode& Extracranial2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Extracranial2);
    SEFluidCircuitNode& Extracranial3 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Extracranial3);
    Extracranial1.GetVolumeBaseline().SetValue(VolumeFractionExtracranialArterioles * bloodVolume_mL, VolumeUnit::mL);
    Extracranial2.GetVolumeBaseline().SetValue(VolumeFractionExtracranialCapillaries * bloodVolume_mL, VolumeUnit::mL);
    Extracranial3.GetVolumeBaseline().SetValue(VolumeFractionExtracranialVenules * bloodVolume_mL, VolumeUnit::mL);
    Extracranial1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    Extracranial2.GetPressure().SetValue(VascularPressureTargetExtracranial, PressureUnit::mmHg);
    Extracranial3.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& Intracranial1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Intracranial1);
    SEFluidCircuitNode& Intracranial2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Intracranial2);
    SEFluidCircuitNode& Intracranial3 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Intracranial3);
    Intracranial1.GetVolumeBaseline().SetValue(VolumeFractionIntracranialArterioles * bloodVolume_mL, VolumeUnit::mL);
    Intracranial2.GetVolumeBaseline().SetValue(VolumeFractionIntracranialCapillaries * bloodVolume_mL, VolumeUnit::mL);
    Intracranial3.GetVolumeBaseline().SetValue(VolumeFractionIntracranialVenules * bloodVolume_mL, VolumeUnit::mL);
    Intracranial1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    Intracranial2.GetPressure().SetValue(VascularPressureTargetIntracranial, PressureUnit::mmHg);
    Intracranial3.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftArm1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftArm1);
    SEFluidCircuitNode& LeftArmBone1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftArmBone1);
    SEFluidCircuitNode& LeftArmFat1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftArmFat1);
    SEFluidCircuitNode& LeftArmMicrovasculature1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftArmMicroVasculature1);
    SEFluidCircuitNode& LeftArmMuscle1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftArmMuscle1);
    SEFluidCircuitNode& LeftArmSkin1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftArmSkin1);
    SEFluidCircuitNode& LeftArm2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftArm2);
    LeftArm1.GetVolumeBaseline().SetValue(VolumeFractionArmArterioles * bloodVolume_mL, VolumeUnit::mL);
    LeftArmBone1.GetVolumeBaseline().SetValue(VolumeFractionArmBone * bloodVolume_mL, VolumeUnit::mL);
    LeftArmFat1.GetVolumeBaseline().SetValue(VolumeFractionArmFat * bloodVolume_mL, VolumeUnit::mL);
    LeftArmMicrovasculature1.GetVolumeBaseline().SetValue(VolumeFractionArmMicrovasculature * bloodVolume_mL, VolumeUnit::mL);
    LeftArmMuscle1.GetVolumeBaseline().SetValue(VolumeFractionArmMuscle * bloodVolume_mL, VolumeUnit::mL);
    LeftArmSkin1.GetVolumeBaseline().SetValue(VolumeFractionArmSkin * bloodVolume_mL, VolumeUnit::mL);
    LeftArm2.GetVolumeBaseline().SetValue(VolumeFractionArmVenules * bloodVolume_mL, VolumeUnit::mL);
    LeftArm1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    LeftArmBone1.GetPressure().SetValue(VascularPressureTargetArmBone, PressureUnit::mmHg);
    LeftArmFat1.GetPressure().SetValue(VascularPressureTargetArmFat, PressureUnit::mmHg);
    LeftArmMicrovasculature1.GetPressure().SetValue(VascularPressureTargetArmMicrovasculature, PressureUnit::mmHg);
    LeftArmMuscle1.GetPressure().SetValue(VascularPressureTargetArmMuscle, PressureUnit::mmHg);
    LeftArmSkin1.GetPressure().SetValue(VascularPressureTargetArmSkin, PressureUnit::mmHg);
    LeftArm2.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftKidney1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftKidney1);
    SEFluidCircuitNode& LeftKidney2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftKidney2);
    SEFluidCircuitNode& LeftKidney3 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftKidney3);
    LeftKidney1.GetVolumeBaseline().SetValue(0.5 * VolumeFractionKidneyArterioles * bloodVolume_mL, VolumeUnit::mL);
    LeftKidney2.GetVolumeBaseline().SetValue(0.5 * VolumeFractionKidneyCapillaries * bloodVolume_mL, VolumeUnit::mL);
    LeftKidney3.GetVolumeBaseline().SetValue(0.5 * VolumeFractionKidneyVenules * bloodVolume_mL, VolumeUnit::mL);
    LeftKidney1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    LeftKidney2.GetPressure().SetValue(VascularPressureTargetKidney, PressureUnit::mmHg);
    LeftKidney3.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftLeg1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftLeg1);
    SEFluidCircuitNode& LeftLegBone1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftLegBone1);
    SEFluidCircuitNode& LeftLegFat1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftLegFat1);
    SEFluidCircuitNode& LeftLegMicrovasculature1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftLegMicroVasculature1);
    SEFluidCircuitNode& LeftLegMuscle1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftLegMuscle1);
    SEFluidCircuitNode& LeftLegSkin1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftLegSkin1);
    SEFluidCircuitNode& LeftLeg2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LeftLeg2);
    LeftLeg1.GetVolumeBaseline().SetValue(VolumeFractionLegArterioles * bloodVolume_mL, VolumeUnit::mL);
    LeftLegBone1.GetVolumeBaseline().SetValue(VolumeFractionLegBone * bloodVolume_mL, VolumeUnit::mL);
    LeftLegFat1.GetVolumeBaseline().SetValue(VolumeFractionLegFat * bloodVolume_mL, VolumeUnit::mL);
    LeftLegMicrovasculature1.GetVolumeBaseline().SetValue(VolumeFractionLegMicrovasculature * bloodVolume_mL, VolumeUnit::mL);
    LeftLegMuscle1.GetVolumeBaseline().SetValue(VolumeFractionLegMuscle * bloodVolume_mL, VolumeUnit::mL);
    LeftLegSkin1.GetVolumeBaseline().SetValue(VolumeFractionLegSkin * bloodVolume_mL, VolumeUnit::mL);
    LeftLeg2.GetVolumeBaseline().SetValue(VolumeFractionLegVenules * bloodVolume_mL, VolumeUnit::mL);
    LeftLeg1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    LeftLegBone1.GetPressure().SetValue(VascularPressureTargetLegBone, PressureUnit::mmHg);
    LeftLegFat1.GetPressure().SetValue(VascularPressureTargetLegFat, PressureUnit::mmHg);
    LeftLegMicrovasculature1.GetPressure().SetValue(VascularPressureTargetLegMicrovasculature, PressureUnit::mmHg);
    LeftLegMuscle1.GetPressure().SetValue(VascularPressureTargetLegMuscle, PressureUnit::mmHg);
    LeftLegSkin1.GetPressure().SetValue(VascularPressureTargetLegSkin, PressureUnit::mmHg);
    LeftLeg2.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& Myocardium1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Myocardium1);
    SEFluidCircuitNode& Myocardium2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Myocardium2);
    SEFluidCircuitNode& Myocardium3 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Myocardium3);
    Myocardium1.GetVolumeBaseline().SetValue(VolumeFractionMyocardiumArterioles * bloodVolume_mL, VolumeUnit::mL);
    Myocardium2.GetVolumeBaseline().SetValue(VolumeFractionMyocardiumCapillaries * bloodVolume_mL, VolumeUnit::mL);
    Myocardium3.GetVolumeBaseline().SetValue(VolumeFractionMyocardiumVenules * bloodVolume_mL, VolumeUnit::mL);
    Myocardium1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    Myocardium2.GetPressure().SetValue(VascularPressureTargetMyocardium, PressureUnit::mmHg);
    Myocardium3.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& PortalVein1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::PortalVein1);

    SEFluidCircuitNode& RightArm1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightArm1);
    SEFluidCircuitNode& RightArmBone1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightArmBone1);
    SEFluidCircuitNode& RightArmFat1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightArmFat1);
    SEFluidCircuitNode& RightArmMicrovasculature1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightArmMicroVasculature1);
    SEFluidCircuitNode& RightArmMuscle1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightArmMuscle1);
    SEFluidCircuitNode& RightArmSkin1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightArmSkin1);
    SEFluidCircuitNode& RightArm2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightArm2);
    RightArm1.GetVolumeBaseline().SetValue(VolumeFractionArmArterioles * bloodVolume_mL, VolumeUnit::mL);
    RightArmBone1.GetVolumeBaseline().SetValue(VolumeFractionArmBone * bloodVolume_mL, VolumeUnit::mL);
    RightArmFat1.GetVolumeBaseline().SetValue(VolumeFractionArmFat * bloodVolume_mL, VolumeUnit::mL);
    RightArmMicrovasculature1.GetVolumeBaseline().SetValue(VolumeFractionArmMicrovasculature * bloodVolume_mL, VolumeUnit::mL);
    RightArmMuscle1.GetVolumeBaseline().SetValue(VolumeFractionArmMuscle * bloodVolume_mL, VolumeUnit::mL);
    RightArmSkin1.GetVolumeBaseline().SetValue(VolumeFractionArmSkin * bloodVolume_mL, VolumeUnit::mL);
    RightArm2.GetVolumeBaseline().SetValue(VolumeFractionArmVenules * bloodVolume_mL, VolumeUnit::mL);
    RightArm1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    RightArmBone1.GetPressure().SetValue(VascularPressureTargetArmBone, PressureUnit::mmHg);
    RightArmFat1.GetPressure().SetValue(VascularPressureTargetArmFat, PressureUnit::mmHg);
    RightArmMicrovasculature1.GetPressure().SetValue(VascularPressureTargetArmMicrovasculature, PressureUnit::mmHg);
    RightArmMuscle1.GetPressure().SetValue(VascularPressureTargetArmMuscle, PressureUnit::mmHg);
    RightArmSkin1.GetPressure().SetValue(VascularPressureTargetArmSkin, PressureUnit::mmHg);
    RightArm2.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& RightKidney1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightKidney1);
    SEFluidCircuitNode& RightKidney2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightKidney2);
    SEFluidCircuitNode& RightKidney3 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightKidney3);
    RightKidney1.GetVolumeBaseline().SetValue(0.5 * VolumeFractionKidneyArterioles * bloodVolume_mL, VolumeUnit::mL);
    RightKidney2.GetVolumeBaseline().SetValue(0.5 * VolumeFractionKidneyCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightKidney3.GetVolumeBaseline().SetValue(0.5 * VolumeFractionKidneyVenules * bloodVolume_mL, VolumeUnit::mL);
    RightKidney1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    RightKidney2.GetPressure().SetValue(VascularPressureTargetKidney, PressureUnit::mmHg);
    RightKidney3.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& RightLeg1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightLeg1);
    SEFluidCircuitNode& RightLegBone1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightLegBone1);
    SEFluidCircuitNode& RightLegFat1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightLegFat1);
    SEFluidCircuitNode& RightLegMicrovasculature1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightLegMicroVasculature1);
    SEFluidCircuitNode& RightLegMuscle1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightLegMuscle1);
    SEFluidCircuitNode& RightLegSkin1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightLegSkin1);
    SEFluidCircuitNode& RightLeg2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::RightLeg2);
    RightLeg1.GetVolumeBaseline().SetValue(VolumeFractionLegArterioles * bloodVolume_mL, VolumeUnit::mL);
    RightLegBone1.GetVolumeBaseline().SetValue(VolumeFractionLegBone * bloodVolume_mL, VolumeUnit::mL);
    RightLegFat1.GetVolumeBaseline().SetValue(VolumeFractionLegFat * bloodVolume_mL, VolumeUnit::mL);
    RightLegMicrovasculature1.GetVolumeBaseline().SetValue(VolumeFractionLegMicrovasculature * bloodVolume_mL, VolumeUnit::mL);
    RightLegMuscle1.GetVolumeBaseline().SetValue(VolumeFractionLegMuscle * bloodVolume_mL, VolumeUnit::mL);
    RightLegSkin1.GetVolumeBaseline().SetValue(VolumeFractionLegSkin * bloodVolume_mL, VolumeUnit::mL);
    RightLeg2.GetVolumeBaseline().SetValue(VolumeFractionLegVenules * bloodVolume_mL, VolumeUnit::mL);
    RightLeg1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    RightLegBone1.GetPressure().SetValue(VascularPressureTargetLegBone, PressureUnit::mmHg);
    RightLegFat1.GetPressure().SetValue(VascularPressureTargetLegFat, PressureUnit::mmHg);
    RightLegMicrovasculature1.GetPressure().SetValue(VascularPressureTargetLegMicrovasculature, PressureUnit::mmHg);
    RightLegMuscle1.GetPressure().SetValue(VascularPressureTargetLegMuscle, PressureUnit::mmHg);
    RightLegSkin1.GetPressure().SetValue(VascularPressureTargetLegSkin, PressureUnit::mmHg);
    RightLeg2.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);


    // Move a little volume from LargeIntestine to Gut1 for substance transport
    // LargeIntestine1.GetVolumeBaseline().Increment(-10, VolumeUnit::mL);
    SEFluidCircuitNode& Gut1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Gut1);
    Gut1.GetVolumeBaseline().SetValue(VolumeFractionGutLiverArterioles * bloodVolume_mL, VolumeUnit::mL);
    Gut1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);

    SEFluidCircuitNode& SmallIntestine1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::SmallIntestine1);
    SmallIntestine1.GetVolumeBaseline().SetValue(VolumeFractionSmallIntestine * bloodVolume_mL, VolumeUnit::mL);
    SmallIntestine1.GetPressure().SetValue(VascularPressureTargetSmallIntestine, PressureUnit::mmHg);

    SEFluidCircuitNode& Splanchnic1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Splanchnic1);
    Splanchnic1.GetVolumeBaseline().SetValue(VolumeFractionSplanchnic * bloodVolume_mL, VolumeUnit::mL);
    Splanchnic1.GetPressure().SetValue(VascularPressureTargetSplanchnic, PressureUnit::mmHg);

    SEFluidCircuitNode& Spleen1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Spleen1);
    Spleen1.GetVolumeBaseline().SetValue(VolumeFractionSpleen * bloodVolume_mL, VolumeUnit::mL);
    Spleen1.GetPressure().SetValue(VascularPressureTargetSpleen, PressureUnit::mmHg);

    SEFluidCircuitNode& LargeIntestine1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::LargeIntestine1);
    LargeIntestine1.GetVolumeBaseline().SetValue(VolumeFractionLargeIntestine * bloodVolume_mL, VolumeUnit::mL);
    LargeIntestine1.GetPressure().SetValue(VascularPressureTargetLargeIntestine, PressureUnit::mmHg);

    SEFluidCircuitNode& Liver1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Liver1);
    SEFluidCircuitNode& Liver2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Liver2);
    Liver1.GetVolumeBaseline().SetValue(VolumeFractionLiver * bloodVolume_mL, VolumeUnit::mL);
    Liver2.GetVolumeBaseline().SetValue(VolumeFractionGutLiverVenules * bloodVolume_mL, VolumeUnit::mL);
    Liver1.GetPressure().SetValue(VascularPressureTargetLiver, PressureUnit::mmHg);
    Liver2.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& Torso1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Torso1);
    SEFluidCircuitNode& TorsoBone1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::TorsoBone1);
    SEFluidCircuitNode& TorsoFat1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::TorsoFat1);
    SEFluidCircuitNode& TorsoMicrovasculature1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::TorsoMicrovasculature1);
    SEFluidCircuitNode& TorsoMuscle1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::TorsoMuscle1);
    SEFluidCircuitNode& TorsoSkin1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::TorsoSkin1);
    SEFluidCircuitNode& Torso2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Torso2);
    Torso1.GetVolumeBaseline().SetValue(VolumeFractionTorsoArterioles * bloodVolume_mL, VolumeUnit::mL);
    TorsoBone1.GetVolumeBaseline().SetValue(VolumeFractionTorsoBone * bloodVolume_mL, VolumeUnit::mL);
    TorsoFat1.GetVolumeBaseline().SetValue(VolumeFractionTorsoFat * bloodVolume_mL, VolumeUnit::mL);
    TorsoMicrovasculature1.GetVolumeBaseline().SetValue(VolumeFractionTorsoMicrovasculature * bloodVolume_mL, VolumeUnit::mL);
    TorsoMuscle1.GetVolumeBaseline().SetValue(VolumeFractionTorsoMuscle * bloodVolume_mL, VolumeUnit::mL);
    TorsoSkin1.GetVolumeBaseline().SetValue(VolumeFractionTorsoSkin * bloodVolume_mL, VolumeUnit::mL);
    Torso2.GetVolumeBaseline().SetValue(VolumeFractionTorsoVenules * bloodVolume_mL, VolumeUnit::mL);
    Torso1.GetPressure().SetValue(VascularPressureTargetArterioles, PressureUnit::mmHg);
    TorsoBone1.GetPressure().SetValue(VascularPressureTargetTorsoBone, PressureUnit::mmHg);
    TorsoFat1.GetPressure().SetValue(VascularPressureTargetTorsoFat, PressureUnit::mmHg);
    TorsoMicrovasculature1.GetPressure().SetValue(VascularPressureTargetTorsoMicrovasculature, PressureUnit::mmHg);
    TorsoMuscle1.GetPressure().SetValue(VascularPressureTargetTorsoMuscle, PressureUnit::mmHg);
    TorsoSkin1.GetPressure().SetValue(VascularPressureTargetTorsoSkin, PressureUnit::mmHg);
    Torso2.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);


    SEFluidCircuitNode& VenaCava1 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::VenaCava1);
    // Move a little volume from VenaCava0 (Vena Cava) to VenaCava1 (Venous Return) for substance transport
    VenaCava1.GetVolumeBaseline().SetValue(VolumeFractionVenaCava * bloodVolume_mL, VolumeUnit::mL);
    VenaCava1.GetPressure().SetValue(VascularPressureTargetVenules, PressureUnit::mmHg);

    SEFluidCircuitNode& VenaCava2 = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::VenaCava2);
    VenaCava1.GetVolumeBaseline().Increment(-50, VolumeUnit::mL);
    VenaCava2.GetVolumeBaseline().SetValue(50, VolumeUnit::mL);
    VenaCava2.GetPressure().SetValue(VascularPressureTargetVenaCava, PressureUnit::mmHg);

    SEFluidCircuitNode& Ground = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Ground);
    Ground.SetAsReferenceNode();
    Ground.GetPressure().SetValue(0.0, PressureUnit::mmHg);

    double blood_mL = 0;
    for (SEFluidCircuitNode* n : cCardiovascular.GetNodes())
    {
      if (n->HasVolumeBaseline())
        blood_mL += n->GetVolumeBaseline(VolumeUnit::mL);
    }
    if (blood_mL > bloodVolume_mL)
      Error("Blood volume greater than total blood volume");

    SEFluidCircuitNode& Pericardium = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::Pericardium1);
    Pericardium.GetVolumeBaseline().SetValue(15.0, VolumeUnit::mL);
    Pericardium.GetPressure().SetValue(1.0, PressureUnit::mmHg);

    SEFluidCircuitNode& AbdominalCavity = cCardiovascular.CreateNode(pulse::ExpandedCardiovascularNode::AbdominalCavity1);
    AbdominalCavity.GetVolumeBaseline().SetValue(10.0, VolumeUnit::mL);
    AbdominalCavity.GetPressure().SetValue(0.0, PressureUnit::mmHg);

    // Create Paths, set switch (diodes), compliances, and resistances where appropriate
    SEFluidCircuitPath& VenaCava1ToRightHeart2 = cCardiovascular.CreatePath(VenaCava1, RightHeart2, pulse::ExpandedCardiovascularPath::VenaCava1ToRightHeart2);
    VenaCava1ToRightHeart2.GetResistanceBaseline().SetValue(ResistanceHeartRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightHeart2ToRightHeart1 = cCardiovascular.CreatePath(RightHeart2, RightHeart1, pulse::ExpandedCardiovascularPath::RightHeart2ToRightHeart1);
    RightHeart2ToRightHeart1.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& RightHeart1ToRightHeart3 = cCardiovascular.CreatePath(RightHeart1, RightHeart3, pulse::ExpandedCardiovascularPath::RightHeart1ToRightHeart3);
    SEFluidCircuitPath& RightHeart3ToGround = cCardiovascular.CreatePath(Ground, RightHeart3, pulse::ExpandedCardiovascularPath::RightHeart3ToGround);
    RightHeart3ToGround.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& RightHeart1ToMainPulmonaryArteries1 = cCardiovascular.CreatePath(RightHeart1, MainPulmonaryArteries1, pulse::ExpandedCardiovascularPath::RightHeart1ToMainPulmonaryArteries1);
    RightHeart1ToMainPulmonaryArteries1.SetNextValve(eGate::Closed);

    SEFluidCircuitPath& MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1 = cCardiovascular.CreatePath(MainPulmonaryArteries1, RightIntermediatePulmonaryArteries1, pulse::ExpandedCardiovascularPath::MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1);
    //MainPulmonaryArteriesToRightIntermediatePulmonaryArteries.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1 = cCardiovascular.CreatePath(RightIntermediatePulmonaryArteries1, RightPulmonaryArteries1, pulse::ExpandedCardiovascularPath::RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1);
    RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1.GetResistanceBaseline().SetValue(ResistancePulmArt, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& RightPulmonaryArteries1ToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedCardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryVeins1);
    RightPulmonaryArteries1ToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightPulmonaryCapillaries1 = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryCapillaries1, pulse::ExpandedCardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryCapillaries1);
    RightPulmonaryArteries1ToRightPulmonaryCapillaries1.GetResistanceBaseline().SetValue(ResistancePulmCapRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightPulmonaryArteries1ToGround = cCardiovascular.CreatePath(RightPulmonaryArteries1, Ground, pulse::ExpandedCardiovascularPath::RightPulmonaryArteries1ToGround);
    RightPulmonaryArteries1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightPulmonaryCapillaries1ToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightPulmonaryCapillaries1, RightPulmonaryVeins1, pulse::ExpandedCardiovascularPath::RightPulmonaryCapillaries1ToRightPulmonaryVeins1);
    RightPulmonaryCapillaries1ToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistancePulmVeinsRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightPulmonaryCapillaries1ToGround = cCardiovascular.CreatePath(RightPulmonaryCapillaries1, Ground, pulse::ExpandedCardiovascularPath::RightPulmonaryCapillaries1ToGround);
    RightPulmonaryCapillaries1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);

    SEFluidCircuitPath& RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1 = cCardiovascular.CreatePath(RightPulmonaryVeins1, RightIntermediatePulmonaryVeins1, pulse::ExpandedCardiovascularPath::RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1);
    RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceHeartLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightPulmonaryVeins1ToGround = cCardiovascular.CreatePath(RightPulmonaryVeins1, Ground, pulse::ExpandedCardiovascularPath::RightPulmonaryVeins1ToGround);
    RightPulmonaryVeins1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightIntermediatePulmonaryVeins1ToLeftHeart2 = cCardiovascular.CreatePath(RightIntermediatePulmonaryVeins1, LeftHeart2, pulse::ExpandedCardiovascularPath::RightIntermediatePulmonaryVeins1ToLeftHeart2);
    //RightIntermediatePulmonaryVeins1ToLeftHeart2.SetNextValve(eGate::Closed);

    SEFluidCircuitPath& MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1 = cCardiovascular.CreatePath(MainPulmonaryArteries1, LeftIntermediatePulmonaryArteries1, pulse::ExpandedCardiovascularPath::MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1);
    //MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1 = cCardiovascular.CreatePath(LeftIntermediatePulmonaryArteries1, LeftPulmonaryArteries1, pulse::ExpandedCardiovascularPath::LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1);
    LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1.GetResistanceBaseline().SetValue(ResistancePulmArt, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedCardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryVeins1);
    LeftPulmonaryArteries1ToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1 = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryCapillaries1, pulse::ExpandedCardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1);
    LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1.GetResistanceBaseline().SetValue(ResistancePulmCapLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToGround = cCardiovascular.CreatePath(LeftPulmonaryArteries1, Ground, pulse::ExpandedCardiovascularPath::LeftPulmonaryArteries1ToGround);
    LeftPulmonaryArteries1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftPulmonaryCapillaries1ToGround = cCardiovascular.CreatePath(LeftPulmonaryCapillaries1, Ground, pulse::ExpandedCardiovascularPath::LeftPulmonaryCapillaries1ToGround);
    LeftPulmonaryCapillaries1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftPulmonaryCapillaries1, LeftPulmonaryVeins1, pulse::ExpandedCardiovascularPath::LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1);
    LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistancePulmVeinsLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1 = cCardiovascular.CreatePath(LeftPulmonaryVeins1, LeftIntermediatePulmonaryVeins1, pulse::ExpandedCardiovascularPath::LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1);
    LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceHeartLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftPulmonaryVeins1ToGround = cCardiovascular.CreatePath(LeftPulmonaryVeins1, Ground, pulse::ExpandedCardiovascularPath::LeftPulmonaryVeins1ToGround);
    LeftPulmonaryVeins1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftIntermediatePulmonaryVeins1ToLeftHeart2 = cCardiovascular.CreatePath(LeftIntermediatePulmonaryVeins1, LeftHeart2, pulse::ExpandedCardiovascularPath::LeftIntermediatePulmonaryVeins1ToLeftHeart2);
    //LeftIntermediatePulmonaryVeins1ToLeftHeart2.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& LeftHeart2ToLeftHeart1 = cCardiovascular.CreatePath(LeftHeart2, LeftHeart1, pulse::ExpandedCardiovascularPath::LeftHeart2ToLeftHeart1);
    LeftHeart2ToLeftHeart1.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& LeftHeart1ToLeftHeart3 = cCardiovascular.CreatePath(LeftHeart1, LeftHeart3, pulse::ExpandedCardiovascularPath::LeftHeart1ToLeftHeart3);

    SEFluidCircuitPath& LeftHeart3ToGround = cCardiovascular.CreatePath(Ground, LeftHeart3, pulse::ExpandedCardiovascularPath::LeftHeart3ToGround);
    LeftHeart3ToGround.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftHeart1ToAorta2 = cCardiovascular.CreatePath(LeftHeart1, Aorta2, pulse::ExpandedCardiovascularPath::LeftHeart1ToAorta2);
    LeftHeart1ToAorta2.SetNextValve(eGate::Closed);
    cCardiovascular.CreatePath(Aorta2, Aorta3, pulse::ExpandedCardiovascularPath::Aorta2ToAorta3);
    SEFluidCircuitPath& Aorta3ToAorta1 = cCardiovascular.CreatePath(Aorta3, Aorta1, pulse::ExpandedCardiovascularPath::Aorta3ToAorta1);
    Aorta3ToAorta1.GetResistanceBaseline().SetValue(ResistanceAorta, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Aorta1ToAorta4 = cCardiovascular.CreatePath(Aorta1, Aorta4, pulse::ExpandedCardiovascularPath::Aorta1ToAorta4);
    Aorta1ToAorta4.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Aorta1ToAorta5 = cCardiovascular.CreatePath(Aorta1, Aorta5, pulse::ExpandedCardiovascularPath::Aorta1ToAorta5);
    Aorta1ToAorta5.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArterioles, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& Aorta5ToExtracranial1 = cCardiovascular.CreatePath(Aorta5, Extracranial1, pulse::ExpandedCardiovascularPath::Aorta5ToExtracranial1);
    SEFluidCircuitPath& Extracranial1ToExtracranial2 = cCardiovascular.CreatePath(Extracranial1, Extracranial2, pulse::ExpandedCardiovascularPath::Extracranial1ToExtracranial2);
    Extracranial1ToExtracranial2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceExtracranial, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Extracranial2ToGround = cCardiovascular.CreatePath(Extracranial2, Ground, pulse::ExpandedCardiovascularPath::Extracranial2ToGround);
    Extracranial2ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Extracranial2ToExtracranial3 = cCardiovascular.CreatePath(Extracranial2, Extracranial3, pulse::ExpandedCardiovascularPath::Extracranial2ToExtracranial3);
    Extracranial2ToExtracranial3.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceExtracranialVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Extracranial3ToVenaCava2 = cCardiovascular.CreatePath(Extracranial3, VenaCava2, pulse::ExpandedCardiovascularPath::Extracranial3ToVenaCava2);

    SEFluidCircuitPath& Aorta5ToIntracranial1 = cCardiovascular.CreatePath(Aorta5, Intracranial1, pulse::ExpandedCardiovascularPath::Aorta5ToIntracranial1);
    SEFluidCircuitPath& Intracranial1ToIntracranial2 = cCardiovascular.CreatePath(Intracranial1, Intracranial2, pulse::ExpandedCardiovascularPath::Intracranial1ToIntracranial2);
    Intracranial1ToIntracranial2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceIntracranial, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Intracranial2ToGround = cCardiovascular.CreatePath(Intracranial2, Ground, pulse::ExpandedCardiovascularPath::Intracranial2ToGround);
    Intracranial2ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Intracranial2ToIntracranial3 = cCardiovascular.CreatePath(Intracranial2, Intracranial3, pulse::ExpandedCardiovascularPath::Intracranial2ToIntracranial3);
    Intracranial2ToIntracranial3.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceIntracranialVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Intracranial3ToVenaCava2 = cCardiovascular.CreatePath(Intracranial3, VenaCava2, pulse::ExpandedCardiovascularPath::Intracranial3ToVenaCava2);

    SEFluidCircuitPath& Aorta5ToGut1 = cCardiovascular.CreatePath(Aorta5, Gut1, pulse::ExpandedCardiovascularPath::Aorta5ToGut1);

    SEFluidCircuitPath& Gut1ToLargeIntestine1 = cCardiovascular.CreatePath(Gut1, LargeIntestine1, pulse::ExpandedCardiovascularPath::Gut1ToLargeIntestine1);
    Gut1ToLargeIntestine1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLargeIntestine, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LargeIntestine1ToGround = cCardiovascular.CreatePath(LargeIntestine1, Ground, pulse::ExpandedCardiovascularPath::LargeIntestine1ToGround);
    LargeIntestine1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LargeIntestine1ToPortalVein1 = cCardiovascular.CreatePath(LargeIntestine1, PortalVein1, pulse::ExpandedCardiovascularPath::LargeIntestine1ToPortalVein1);
    LargeIntestine1ToPortalVein1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLargeIntestineVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& Aorta5ToLeftArm1 = cCardiovascular.CreatePath(Aorta5, LeftArm1, pulse::ExpandedCardiovascularPath::Aorta5ToLeftArm1);
    SEFluidCircuitPath& LeftArm1ToLeftArmBone1 = cCardiovascular.CreatePath(LeftArm1, LeftArmBone1, pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmBone1);
    LeftArm1ToLeftArmBone1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmBone, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArmBone1ToGround = cCardiovascular.CreatePath(LeftArmBone1, Ground, pulse::ExpandedCardiovascularPath::LeftArmBone1ToGround);
    LeftArmBone1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftArmBone1ToLeftArm2 = cCardiovascular.CreatePath(LeftArmBone1, LeftArm2, pulse::ExpandedCardiovascularPath::LeftArmBone1ToLeftArm2);
    LeftArmBone1ToLeftArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmBoneVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArm1ToLeftArmFat1 = cCardiovascular.CreatePath(LeftArm1, LeftArmFat1, pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmFat1);
    LeftArm1ToLeftArmFat1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmFat, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArmFat1ToGround = cCardiovascular.CreatePath(LeftArmFat1, Ground, pulse::ExpandedCardiovascularPath::LeftArmFat1ToGround);
    LeftArmFat1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftArmFat1ToLeftArm2 = cCardiovascular.CreatePath(LeftArmFat1, LeftArm2, pulse::ExpandedCardiovascularPath::LeftArmFat1ToLeftArm2);
    LeftArmFat1ToLeftArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmFatVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArm1ToLeftArmMicrovasculature1 = cCardiovascular.CreatePath(LeftArm1, LeftArmMicrovasculature1, pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmMicrovasculature1);
    LeftArm1ToLeftArmMicrovasculature1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmMicrovasculature, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArmMicrovasculature1ToGround = cCardiovascular.CreatePath(LeftArmMicrovasculature1, Ground, pulse::ExpandedCardiovascularPath::LeftArmMicrovasculature1ToGround);
    LeftArmMicrovasculature1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftArmMicrovasculature1ToLeftArm2 = cCardiovascular.CreatePath(LeftArmMicrovasculature1, LeftArm2, pulse::ExpandedCardiovascularPath::LeftArmMicrovasculature1ToLeftArm2);
    LeftArmMicrovasculature1ToLeftArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmMicrovasculatureVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArm1ToLeftArmMuscle1 = cCardiovascular.CreatePath(LeftArm1, LeftArmMuscle1, pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmMuscle1);
    LeftArm1ToLeftArmMuscle1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmMuscle, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArmMuscle1ToGround = cCardiovascular.CreatePath(LeftArmMuscle1, Ground, pulse::ExpandedCardiovascularPath::LeftArmMuscle1ToGround);
    LeftArmMuscle1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftArmMuscle1ToLeftArm2 = cCardiovascular.CreatePath(LeftArmMuscle1, LeftArm2, pulse::ExpandedCardiovascularPath::LeftArmMuscle1ToLeftArm2);
    LeftArmMuscle1ToLeftArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmMuscleVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArm1ToLeftArmSkin1 = cCardiovascular.CreatePath(LeftArm1, LeftArmSkin1, pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmSkin1);
    LeftArm1ToLeftArmSkin1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmSkin, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArmSkin1ToGround = cCardiovascular.CreatePath(LeftArmSkin1, Ground, pulse::ExpandedCardiovascularPath::LeftArmSkin1ToGround);
    LeftArmSkin1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftArmSkin1ToLeftArm2 = cCardiovascular.CreatePath(LeftArmSkin1, LeftArm2, pulse::ExpandedCardiovascularPath::LeftArmSkin1ToLeftArm2);
    LeftArmSkin1ToLeftArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmSkinVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArm2ToVenaCava2 = cCardiovascular.CreatePath(LeftArm2, VenaCava2, pulse::ExpandedCardiovascularPath::LeftArm2ToVenaCava2);

    SEFluidCircuitPath& Aorta5ToLeftKidney1 = cCardiovascular.CreatePath(Aorta5, LeftKidney1, pulse::ExpandedCardiovascularPath::Aorta5ToLeftKidney1);
    SEFluidCircuitPath& LeftKidney1ToLeftKidney2 = cCardiovascular.CreatePath(LeftKidney1, LeftKidney2, pulse::ExpandedCardiovascularPath::LeftKidney1ToLeftKidney2);
    LeftKidney1ToLeftKidney2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceKidney, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftKidney2ToGround = cCardiovascular.CreatePath(LeftKidney2, Ground, pulse::ExpandedCardiovascularPath::LeftKidney2ToGround);
    LeftKidney2ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftKidney2ToLeftKidney3 = cCardiovascular.CreatePath(LeftKidney2, LeftKidney3, pulse::ExpandedCardiovascularPath::LeftKidney2ToLeftKidney3);
    LeftKidney2ToLeftKidney3.GetResistanceBaseline().SetValue(ResistanceKidneyVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftKidney3ToVenaCava2 = cCardiovascular.CreatePath(LeftKidney3, VenaCava2, pulse::ExpandedCardiovascularPath::LeftKidney3ToVenaCava2);

    SEFluidCircuitPath& Aorta5ToLeftLeg1 = cCardiovascular.CreatePath(Aorta5, LeftLeg1, pulse::ExpandedCardiovascularPath::Aorta5ToLeftLeg1);
    SEFluidCircuitPath& LeftLeg1ToLeftLegBone1 = cCardiovascular.CreatePath(LeftLeg1, LeftLegBone1, pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegBone1);
    LeftLeg1ToLeftLegBone1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegBone, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLegBone1ToGround = cCardiovascular.CreatePath(LeftLegBone1, Ground, pulse::ExpandedCardiovascularPath::LeftLegBone1ToGround);
    LeftLegBone1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftLegBone1ToLeftLeg2 = cCardiovascular.CreatePath(LeftLegBone1, LeftLeg2, pulse::ExpandedCardiovascularPath::LeftLegBone1ToLeftLeg2);
    LeftLegBone1ToLeftLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegBoneVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLeg1ToLeftLegFat1 = cCardiovascular.CreatePath(LeftLeg1, LeftLegFat1, pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegFat1);
    LeftLeg1ToLeftLegFat1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegFat, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLegFat1ToGround = cCardiovascular.CreatePath(LeftLegFat1, Ground, pulse::ExpandedCardiovascularPath::LeftLegFat1ToGround);
    LeftLegFat1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftLegFat1ToLeftLeg2 = cCardiovascular.CreatePath(LeftLegFat1, LeftLeg2, pulse::ExpandedCardiovascularPath::LeftLegFat1ToLeftLeg2);
    LeftLegFat1ToLeftLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegFatVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLeg1ToLeftLegMicrovasculature1 = cCardiovascular.CreatePath(LeftLeg1, LeftLegMicrovasculature1, pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegMicrovasculature1);
    LeftLeg1ToLeftLegMicrovasculature1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegMicrovasculature, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLegMicrovasculature1ToGround = cCardiovascular.CreatePath(LeftLegMicrovasculature1, Ground, pulse::ExpandedCardiovascularPath::LeftLegMicrovasculature1ToGround);
    LeftLegMicrovasculature1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftLegMicrovasculature1ToLeftLeg2 = cCardiovascular.CreatePath(LeftLegMicrovasculature1, LeftLeg2, pulse::ExpandedCardiovascularPath::LeftLegMicrovasculature1ToLeftLeg2);
    LeftLegMicrovasculature1ToLeftLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegMicrovasculatureVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLeg1ToLeftLegMuscle1 = cCardiovascular.CreatePath(LeftLeg1, LeftLegMuscle1, pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegMuscle1);
    LeftLeg1ToLeftLegMuscle1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegMuscle, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLegMuscle1ToGround = cCardiovascular.CreatePath(LeftLegMuscle1, Ground, pulse::ExpandedCardiovascularPath::LeftLegMuscle1ToGround);
    LeftLegMuscle1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftLegMuscle1ToLeftLeg2 = cCardiovascular.CreatePath(LeftLegMuscle1, LeftLeg2, pulse::ExpandedCardiovascularPath::LeftLegMuscle1ToLeftLeg2);
    LeftLegMuscle1ToLeftLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegMuscleVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLeg1ToLeftLegSkin1 = cCardiovascular.CreatePath(LeftLeg1, LeftLegSkin1, pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegSkin1);
    LeftLeg1ToLeftLegSkin1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegSkin, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLegSkin1ToGround = cCardiovascular.CreatePath(LeftLegSkin1, Ground, pulse::ExpandedCardiovascularPath::LeftLegSkin1ToGround);
    LeftLegSkin1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftLegSkin1ToLeftLeg2 = cCardiovascular.CreatePath(LeftLegSkin1, LeftLeg2, pulse::ExpandedCardiovascularPath::LeftLegSkin1ToLeftLeg2);
    LeftLegSkin1ToLeftLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegSkinVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLeg2ToVenaCava2 = cCardiovascular.CreatePath(LeftLeg2, VenaCava2, pulse::ExpandedCardiovascularPath::LeftLeg2ToVenaCava2);

    SEFluidCircuitPath& Gut1ToLiver1 = cCardiovascular.CreatePath(Gut1, Liver1, pulse::ExpandedCardiovascularPath::Gut1ToLiver1);
    Gut1ToLiver1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLiver, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Liver1ToGround = cCardiovascular.CreatePath(Liver1, Ground, pulse::ExpandedCardiovascularPath::Liver1ToGround);
    Liver1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& PortalVein1ToLiver1 = cCardiovascular.CreatePath(PortalVein1, Liver1, pulse::ExpandedCardiovascularPath::PortalVein1ToLiver1);
    PortalVein1ToLiver1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistancePortalVein, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Liver1ToLiver2 = cCardiovascular.CreatePath(Liver1, Liver2, pulse::ExpandedCardiovascularPath::Liver1ToLiver2);
    Liver1ToLiver2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLiverVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Liver2ToVenaCava2 = cCardiovascular.CreatePath(Liver2, VenaCava2, pulse::ExpandedCardiovascularPath::Liver2ToVenaCava2);

    SEFluidCircuitPath& Aorta5ToMyocardium1 = cCardiovascular.CreatePath(Aorta5, Myocardium1, pulse::ExpandedCardiovascularPath::Aorta5ToMyocardium1);
    SEFluidCircuitPath& Myocardium1ToMyocardium2 = cCardiovascular.CreatePath(Myocardium1, Myocardium2, pulse::ExpandedCardiovascularPath::Myocardium1ToMyocardium2);
    Myocardium1ToMyocardium2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceMyocardium, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Myocardium2ToGround = cCardiovascular.CreatePath(Myocardium2, Ground, pulse::ExpandedCardiovascularPath::Myocardium2ToGround);
    Myocardium2ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Myocardium2ToMyocardium3 = cCardiovascular.CreatePath(Myocardium2, Myocardium3, pulse::ExpandedCardiovascularPath::Myocardium2ToMyocardium3);
    Myocardium2ToMyocardium3.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceMyocardiumVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Myocardium3ToVenaCava2 = cCardiovascular.CreatePath(Myocardium3, VenaCava2, pulse::ExpandedCardiovascularPath::Myocardium3ToVenaCava2);

    SEFluidCircuitPath& PericardiumToGround = cCardiovascular.CreatePath(Pericardium, Ground, pulse::ExpandedCardiovascularPath::Pericardium1ToGround);
    SEFluidCircuitPath& GroundToPericardium = cCardiovascular.CreatePath(Ground, Pericardium, pulse::ExpandedCardiovascularPath::GroundToPericardium1);
    GroundToPericardium.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SEFluidCircuitPath& Aorta5ToRightArm1 = cCardiovascular.CreatePath(Aorta5, RightArm1, pulse::ExpandedCardiovascularPath::Aorta5ToRightArm1);
    SEFluidCircuitPath& RightArm1ToRightArmBone1 = cCardiovascular.CreatePath(RightArm1, RightArmBone1, pulse::ExpandedCardiovascularPath::RightArm1ToRightArmBone1);
    RightArm1ToRightArmBone1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmBone, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArmBone1ToGround = cCardiovascular.CreatePath(RightArmBone1, Ground, pulse::ExpandedCardiovascularPath::RightArmBone1ToGround);
    RightArmBone1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightArmBone1ToRightArm2 = cCardiovascular.CreatePath(RightArmBone1, RightArm2, pulse::ExpandedCardiovascularPath::RightArmBone1ToRightArm2);
    RightArmBone1ToRightArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmBoneVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArm1ToRightArmFat1 = cCardiovascular.CreatePath(RightArm1, RightArmFat1, pulse::ExpandedCardiovascularPath::RightArm1ToRightArmFat1);
    RightArm1ToRightArmFat1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmFat, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArmFat1ToGround = cCardiovascular.CreatePath(RightArmFat1, Ground, pulse::ExpandedCardiovascularPath::RightArmFat1ToGround);
    RightArmFat1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightArmFat1ToRightArm2 = cCardiovascular.CreatePath(RightArmFat1, RightArm2, pulse::ExpandedCardiovascularPath::RightArmFat1ToRightArm2);
    RightArmFat1ToRightArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmFatVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArm1ToRightArmMicrovasculature1 = cCardiovascular.CreatePath(RightArm1, RightArmMicrovasculature1, pulse::ExpandedCardiovascularPath::RightArm1ToRightArmMicrovasculature1);
    RightArm1ToRightArmMicrovasculature1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmMicrovasculature, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArmMicrovasculature1ToGround = cCardiovascular.CreatePath(RightArmMicrovasculature1, Ground, pulse::ExpandedCardiovascularPath::RightArmMicrovasculature1ToGround);
    RightArmMicrovasculature1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightArmMicrovasculature1ToRightArm2 = cCardiovascular.CreatePath(RightArmMicrovasculature1, RightArm2, pulse::ExpandedCardiovascularPath::RightArmMicrovasculature1ToRightArm2);
    RightArmMicrovasculature1ToRightArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmMicrovasculatureVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArm1ToRightArmMuscle1 = cCardiovascular.CreatePath(RightArm1, RightArmMuscle1, pulse::ExpandedCardiovascularPath::RightArm1ToRightArmMuscle1);
    RightArm1ToRightArmMuscle1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmMuscle, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArmMuscle1ToGround = cCardiovascular.CreatePath(RightArmMuscle1, Ground, pulse::ExpandedCardiovascularPath::RightArmMuscle1ToGround);
    RightArmMuscle1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightArmMuscle1ToRightArm2 = cCardiovascular.CreatePath(RightArmMuscle1, RightArm2, pulse::ExpandedCardiovascularPath::RightArmMuscle1ToRightArm2);
    RightArmMuscle1ToRightArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmMuscleVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArm1ToRightArmSkin1 = cCardiovascular.CreatePath(RightArm1, RightArmSkin1, pulse::ExpandedCardiovascularPath::RightArm1ToRightArmSkin1);
    RightArm1ToRightArmSkin1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmSkin, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArmSkin1ToGround = cCardiovascular.CreatePath(RightArmSkin1, Ground, pulse::ExpandedCardiovascularPath::RightArmSkin1ToGround);
    RightArmSkin1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightArmSkin1ToRightArm2 = cCardiovascular.CreatePath(RightArmSkin1, RightArm2, pulse::ExpandedCardiovascularPath::RightArmSkin1ToRightArm2);
    RightArmSkin1ToRightArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmSkinVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArm2ToVenaCava2 = cCardiovascular.CreatePath(RightArm2, VenaCava2, pulse::ExpandedCardiovascularPath::RightArm2ToVenaCava2);

    SEFluidCircuitPath& Aorta5ToRightKidney1 = cCardiovascular.CreatePath(Aorta5, RightKidney1, pulse::ExpandedCardiovascularPath::Aorta5ToRightKidney1);
    SEFluidCircuitPath& RightKidney1ToRightKidney2 = cCardiovascular.CreatePath(RightKidney1, RightKidney2, pulse::ExpandedCardiovascularPath::RightKidney1ToRightKidney2);
    RightKidney1ToRightKidney2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceKidney, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightKidney2ToGround = cCardiovascular.CreatePath(RightKidney2, Ground, pulse::ExpandedCardiovascularPath::RightKidney2ToGround);
    RightKidney2ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightKidney2ToRightKidney3 = cCardiovascular.CreatePath(RightKidney2, RightKidney3, pulse::ExpandedCardiovascularPath::RightKidney2ToRightKidney3);
    RightKidney2ToRightKidney3.GetResistanceBaseline().SetValue(ResistanceKidneyVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightKidney3ToVenaCava2 = cCardiovascular.CreatePath(RightKidney3, VenaCava2, pulse::ExpandedCardiovascularPath::RightKidney3ToVenaCava2);

    SEFluidCircuitPath& Aorta5ToRightLeg1 = cCardiovascular.CreatePath(Aorta5, RightLeg1, pulse::ExpandedCardiovascularPath::Aorta5ToRightLeg1);
    SEFluidCircuitPath& RightLeg1ToRightLegBone1 = cCardiovascular.CreatePath(RightLeg1, RightLegBone1, pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegBone1);
    RightLeg1ToRightLegBone1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegBone, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLegBone1ToGround = cCardiovascular.CreatePath(RightLegBone1, Ground, pulse::ExpandedCardiovascularPath::RightLegBone1ToGround);
    RightLegBone1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightLegBone1ToRightLeg2 = cCardiovascular.CreatePath(RightLegBone1, RightLeg2, pulse::ExpandedCardiovascularPath::RightLegBone1ToRightLeg2);
    RightLegBone1ToRightLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegBoneVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLeg1ToRightLegFat1 = cCardiovascular.CreatePath(RightLeg1, RightLegFat1, pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegFat1);
    RightLeg1ToRightLegFat1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegFat, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLegFat1ToGround = cCardiovascular.CreatePath(RightLegFat1, Ground, pulse::ExpandedCardiovascularPath::RightLegFat1ToGround);
    RightLegFat1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightLegFat1ToRightLeg2 = cCardiovascular.CreatePath(RightLegFat1, RightLeg2, pulse::ExpandedCardiovascularPath::RightLegFat1ToRightLeg2);
    RightLegFat1ToRightLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegFatVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLeg1ToRightLegMicrovasculature1 = cCardiovascular.CreatePath(RightLeg1, RightLegMicrovasculature1, pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegMicrovasculature1);
    RightLeg1ToRightLegMicrovasculature1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegMicrovasculature, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLegMicrovasculature1ToGround = cCardiovascular.CreatePath(RightLegMicrovasculature1, Ground, pulse::ExpandedCardiovascularPath::RightLegMicrovasculature1ToGround);
    RightLegMicrovasculature1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightLegMicrovasculature1ToRightLeg2 = cCardiovascular.CreatePath(RightLegMicrovasculature1, RightLeg2, pulse::ExpandedCardiovascularPath::RightLegMicrovasculature1ToRightLeg2);
    RightLegMicrovasculature1ToRightLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegMicrovasculatureVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLeg1ToRightLegMuscle1 = cCardiovascular.CreatePath(RightLeg1, RightLegMuscle1, pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegMuscle1);
    RightLeg1ToRightLegMuscle1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegMuscle, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLegMuscle1ToGround = cCardiovascular.CreatePath(RightLegMuscle1, Ground, pulse::ExpandedCardiovascularPath::RightLegMuscle1ToGround);
    RightLegMuscle1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightLegMuscle1ToRightLeg2 = cCardiovascular.CreatePath(RightLegMuscle1, RightLeg2, pulse::ExpandedCardiovascularPath::RightLegMuscle1ToRightLeg2);
    RightLegMuscle1ToRightLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegMuscleVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLeg1ToRightLegSkin1 = cCardiovascular.CreatePath(RightLeg1, RightLegSkin1, pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegSkin1);
    RightLeg1ToRightLegSkin1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegSkin, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLegSkin1ToGround = cCardiovascular.CreatePath(RightLegSkin1, Ground, pulse::ExpandedCardiovascularPath::RightLegSkin1ToGround);
    RightLegSkin1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightLegSkin1ToRightLeg2 = cCardiovascular.CreatePath(RightLegSkin1, RightLeg2, pulse::ExpandedCardiovascularPath::RightLegSkin1ToRightLeg2);
    RightLegSkin1ToRightLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegSkinVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLeg2ToVenaCava2 = cCardiovascular.CreatePath(RightLeg2, VenaCava2, pulse::ExpandedCardiovascularPath::RightLeg2ToVenaCava2);

    SEFluidCircuitPath& Gut1ToSmallIntestine1 = cCardiovascular.CreatePath(Gut1, SmallIntestine1, pulse::ExpandedCardiovascularPath::Gut1ToSmallIntestine1);
    Gut1ToSmallIntestine1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSmallIntestine, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& SmallIntestine1ToGround = cCardiovascular.CreatePath(SmallIntestine1, Ground, pulse::ExpandedCardiovascularPath::SmallIntestine1ToGround);
    SmallIntestine1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& SmallIntestine1ToPortalVein1 = cCardiovascular.CreatePath(SmallIntestine1, PortalVein1, pulse::ExpandedCardiovascularPath::SmallIntestine1ToPortalVein1);
    SmallIntestine1ToPortalVein1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSmallIntestineVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Gut1ToSplanchnic1 = cCardiovascular.CreatePath(Gut1, Splanchnic1, pulse::ExpandedCardiovascularPath::Gut1ToSplanchnic1);
    Gut1ToSplanchnic1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSplanchnic, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Splanchnic1ToGround = cCardiovascular.CreatePath(Splanchnic1, Ground, pulse::ExpandedCardiovascularPath::Splanchnic1ToGround);
    Splanchnic1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Splanchnic1ToPortalVein1 = cCardiovascular.CreatePath(Splanchnic1, PortalVein1, pulse::ExpandedCardiovascularPath::Splanchnic1ToPortalVein1);
    Splanchnic1ToPortalVein1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSplanchnicVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Gut1ToSpleen1 = cCardiovascular.CreatePath(Gut1, Spleen1, pulse::ExpandedCardiovascularPath::Gut1ToSpleen1);
    Gut1ToSpleen1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSpleen, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& SpleenToGround = cCardiovascular.CreatePath(Spleen1, Ground, pulse::ExpandedCardiovascularPath::Spleen1ToGround);
    SpleenToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Spleen1ToPortalVein1 = cCardiovascular.CreatePath(Spleen1, PortalVein1, pulse::ExpandedCardiovascularPath::Spleen1ToPortalVein1);
    Spleen1ToPortalVein1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSpleenVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& Aorta5ToTorso1 = cCardiovascular.CreatePath(Aorta5, Torso1, pulse::ExpandedCardiovascularPath::Aorta5ToTorso1);
    SEFluidCircuitPath& Torso1ToTorsoBone1 = cCardiovascular.CreatePath(Torso1, TorsoBone1, pulse::ExpandedCardiovascularPath::Torso1ToTorsoBone1);
    Torso1ToTorsoBone1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoBone, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& TorsoBone1ToGround = cCardiovascular.CreatePath(TorsoBone1, Ground, pulse::ExpandedCardiovascularPath::TorsoBone1ToGround);
    TorsoBone1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& TorsoBone1ToTorso2 = cCardiovascular.CreatePath(TorsoBone1, Torso2, pulse::ExpandedCardiovascularPath::TorsoBone1ToTorso2);
    TorsoBone1ToTorso2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoBoneVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Torso1ToTorsoFat1 = cCardiovascular.CreatePath(Torso1, TorsoFat1, pulse::ExpandedCardiovascularPath::Torso1ToTorsoFat1);
    Torso1ToTorsoFat1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoFat, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& TorsoFat1ToGround = cCardiovascular.CreatePath(TorsoFat1, Ground, pulse::ExpandedCardiovascularPath::TorsoFat1ToGround);
    TorsoFat1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& TorsoFat1ToTorso2 = cCardiovascular.CreatePath(TorsoFat1, Torso2, pulse::ExpandedCardiovascularPath::TorsoFat1ToTorso2);
    TorsoFat1ToTorso2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoFatVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Torso1ToTorsoMicrovasculature1 = cCardiovascular.CreatePath(Torso1, TorsoMicrovasculature1, pulse::ExpandedCardiovascularPath::Torso1ToTorsoMicrovasculature1);
    Torso1ToTorsoMicrovasculature1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoMicrovasculature, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& TorsoMicrovasculature1ToGround = cCardiovascular.CreatePath(TorsoMicrovasculature1, Ground, pulse::ExpandedCardiovascularPath::TorsoMicrovasculature1ToGround);
    TorsoMicrovasculature1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& TorsoMicrovasculature1ToTorso2 = cCardiovascular.CreatePath(TorsoMicrovasculature1, Torso2, pulse::ExpandedCardiovascularPath::TorsoMicrovasculature1ToTorso2);
    TorsoMicrovasculature1ToTorso2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoMicrovasculatureVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Torso1ToTorsoMuscle1 = cCardiovascular.CreatePath(Torso1, TorsoMuscle1, pulse::ExpandedCardiovascularPath::Torso1ToTorsoMuscle1);
    Torso1ToTorsoMuscle1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoMuscle, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& TorsoMuscle1ToGround = cCardiovascular.CreatePath(TorsoMuscle1, Ground, pulse::ExpandedCardiovascularPath::TorsoMuscle1ToGround);
    TorsoMuscle1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& TorsoMuscle1ToTorso2 = cCardiovascular.CreatePath(TorsoMuscle1, Torso2, pulse::ExpandedCardiovascularPath::TorsoMuscle1ToTorso2);
    TorsoMuscle1ToTorso2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoMuscleVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Torso1ToTorsoSkin1 = cCardiovascular.CreatePath(Torso1, TorsoSkin1, pulse::ExpandedCardiovascularPath::Torso1ToTorsoSkin1);
    Torso1ToTorsoSkin1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoSkin, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& TorsoSkin1ToGround = cCardiovascular.CreatePath(TorsoSkin1, Ground, pulse::ExpandedCardiovascularPath::TorsoSkin1ToGround);
    TorsoSkin1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& TorsoSkin1ToTorso2 = cCardiovascular.CreatePath(TorsoSkin1, Torso2, pulse::ExpandedCardiovascularPath::TorsoSkin1ToTorso2);
    TorsoSkin1ToTorso2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceTorsoSkinVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Torso2ToVenaCava2 = cCardiovascular.CreatePath(Torso2, VenaCava2, pulse::ExpandedCardiovascularPath::Torso2ToVenaCava2);

    SEFluidCircuitPath& AbdominalCavityToGround = cCardiovascular.CreatePath(AbdominalCavity, Ground, pulse::ExpandedCardiovascularPath::AbdominalCavity1ToGround);
    SEFluidCircuitPath& GroundToAbdominalCavity = cCardiovascular.CreatePath(Ground, AbdominalCavity, pulse::ExpandedCardiovascularPath::GroundToAbdominalCavity1);
    GroundToAbdominalCavity.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SEFluidCircuitPath& GroundToAorta4 = cCardiovascular.CreatePath(Ground, Aorta4, pulse::ExpandedCardiovascularPath::GroundToAorta4);
    GroundToAorta4.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& VenaCava2ToVenaCava1 = cCardiovascular.CreatePath(VenaCava2, VenaCava1, pulse::ExpandedCardiovascularPath::VenaCava2ToVenaCava1);
    VenaCava2ToVenaCava1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceVenules, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& VenaCava1ToGround = cCardiovascular.CreatePath(VenaCava1, Ground, pulse::ExpandedCardiovascularPath::VenaCava1ToGround);
    VenaCava1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    //SEFluidCircuitPath& VenaCava2ToGround = cCardiovascular.CreatePath(VenaCava2, Ground, pulse::ExpandedCardiovascularPath::VenaCava2ToGround);
    //VenaCava2ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& IVToVenaCava1 = cCardiovascular.CreatePath(Ground, VenaCava1, pulse::ExpandedCardiovascularPath::IVToVenaCava1);
    IVToVenaCava1.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);


    //Here we add compliance to all in / out nodes of each region
    std::vector<SEFluidCircuitNode*> list_compliances;
    list_compliances.push_back(&Extracranial1);
    list_compliances.push_back(&Intracranial1);
    list_compliances.push_back(&Myocardium1);
    list_compliances.push_back(&LeftArm1);
    list_compliances.push_back(&RightArm1);
    list_compliances.push_back(&Gut1);
    list_compliances.push_back(&LeftLeg1);
    list_compliances.push_back(&RightLeg1);
    list_compliances.push_back(&Torso1);


    list_compliances.push_back(&Extracranial3);
    list_compliances.push_back(&Intracranial3);
    list_compliances.push_back(&Myocardium3);
    list_compliances.push_back(&LeftArm2);
    list_compliances.push_back(&RightArm2);
    list_compliances.push_back(&Liver2);
    list_compliances.push_back(&LeftLeg2);
    list_compliances.push_back(&RightLeg2);
    list_compliances.push_back(&Torso2);
    list_compliances.push_back(&LeftKidney1);
    list_compliances.push_back(&LeftKidney3);
    list_compliances.push_back(&RightKidney1);
    list_compliances.push_back(&RightKidney3);

    for (SEFluidCircuitNode* node : list_compliances) {
      SEFluidCircuitPath& path = cCardiovascular.CreatePath(*node, Ground, node->GetName() + "ToGround");
      //Info("Pressure [mmHg] = " + std::to_string(node->GetPressure().GetValue(PressureUnit::mmHg)) + " VolumeBaseline [mL] = " + std::to_string(node->GetVolumeBaseline().GetValue(VolumeUnit::mL)) + " " + node->GetName());
      path.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    }

    // Compute compliances from target pressures and baseline volumes
    for (SEFluidCircuitPath* p : cCardiovascular.GetPaths())
    {
      if (p->HasComplianceBaseline())
      {
        SEFluidCircuitNode& src = p->GetSourceNode();
        if (!src.HasVolumeBaseline())
          Fatal("Compliance paths must have a volume baseline.");
        double pressure = src.GetPressure(PressureUnit::mmHg);
        double volume = src.GetVolumeBaseline(VolumeUnit::mL);
        // p->GetComplianceBaseline().SetValue(volume / pressure, VolumePerPressureUnit::mL_Per_mmHg);
        // C = (V_total - V_unstressed) / P. 
        // Since V_total = V_unstressed + V_stressed -> C = V_stressed/P
        //C = (V_total - V_unstressed) / P. Since V_total = V_unstressed + V_stressed -> C = V_stressed/P
        p->GetComplianceBaseline().SetValue(volume * StressedVolume / pressure, VolumePerPressureUnit::mL_Per_mmHg);

      }
    }

    list_compliances.clear();
    list_compliances.push_back(&Torso1);
    list_compliances.push_back(&Torso2);

    for (SEFluidCircuitNode* node : list_compliances) {
      SEFluidCircuitPath* path = cCardiovascular.GetPath(node->GetName() + "ToGround");
      double compliance = path->GetComplianceBaseline().GetValue(VolumePerPressureUnit::mL_Per_mmHg);
      path->GetComplianceBaseline().SetValue(compliance * 0.7, VolumePerPressureUnit::mL_Per_mmHg);
    }

    list_compliances.clear();
    list_compliances.push_back(&LeftPulmonaryArteries1);
    list_compliances.push_back(&LeftPulmonaryCapillaries1);
    list_compliances.push_back(&LeftPulmonaryVeins1);
    list_compliances.push_back(&RightPulmonaryArteries1);
    list_compliances.push_back(&RightPulmonaryCapillaries1);
    list_compliances.push_back(&RightPulmonaryVeins1);
    for (SEFluidCircuitNode* node : list_compliances) {
      SEFluidCircuitPath* path = cCardiovascular.GetPath(node->GetName() + "ToGround");
      double compliance = path->GetComplianceBaseline().GetValue(VolumePerPressureUnit::mL_Per_mmHg);
      path->GetComplianceBaseline().SetValue(compliance * 0.3, VolumePerPressureUnit::mL_Per_mmHg);
    }

    double compliance;
    for (SEFluidCircuitNode* node : list_compliances) {
      compliance = cCardiovascular.GetPath(node->GetName() + "ToGround")->GetComplianceBaseline().GetValue(VolumePerPressureUnit::mL_Per_mmHg);
      Info("Compliance [mL/mmHg] = " + std::to_string(compliance) + " Pressure [mmHg] = " + std::to_string(node->GetPressure().GetValue(PressureUnit::mmHg)) + " VolumeBaseline [mL] = " + std::to_string(node->GetVolumeBaseline().GetValue(VolumeUnit::mL)) + " " + node->GetName());
    }

    // Hearts and pericardium have special compliance computations
    double InitialComplianceHeartRight = 1.0 / 0.0243;
    double InitialComplianceHeartLeft = 1.0 / 0.049;
    // Volumes are initialized from the volume baselines. The heart volume initialization is a little tricky. To much prime and the 
    // initial pressure wave will be devastating to the rest of the CV system during the first contraction phase. Too little prime 
    // and there will be issues with available flow as the elastance decreases during the first relaxation phase.
    // The 1/4 full initialization gives decent results.
    RightHeart1ToRightHeart3.GetComplianceBaseline().SetValue(InitialComplianceHeartRight, VolumePerPressureUnit::mL_Per_mmHg);
    LeftHeart1ToLeftHeart3.GetComplianceBaseline().SetValue(InitialComplianceHeartLeft, VolumePerPressureUnit::mL_Per_mmHg);
    PericardiumToGround.GetComplianceBaseline().SetValue(100.0, VolumePerPressureUnit::mL_Per_mmHg);

    //Make these polarized, so chest compression can't give a negative volume
    RightHeart1ToRightHeart3.SetNextPolarizedState(eGate::Closed);
    LeftHeart1ToLeftHeart3.SetNextPolarizedState(eGate::Closed);

    // todo: Volume modifiers are mainly used for compensating the blood volume changes(loss) from vasculature-tissue
    //  interactions. Could be added to the circuit optimizer for optimizing the circuit parameters together with path
    //  modifiers in future.
    double VolumeModifierAorta = 1.189104;//1.087927;
    double VolumeModifierArm = 1.159737;
    double VolumeModifierArmMuscle = 1.287308;
    //double VolumeModifierArmMuscle = 1;
    double VolumeModifierHead = 1.036344;
    double VolumeModifierKidney = 0.703967;
    double VolumeModifierLargeIntestine = 1.159424;
    double VolumeModifierLeg = 1.159737;
    double VolumeModifierLegMuscle = 1.287308;
    double VolumeModifierLiver = 1.148039;
    double VolumeModifierMyocard = 1.15973;
    double VolumeModifierPulmArtL = 1.04056;
    double VolumeModifierPulmCapL = 0.782056;
    double VolumeModifierPulmVeinL = 0.615565;
    double VolumeModifierPulmArtR = 0.941035;
    double VolumeModifierPulmCapR = 0.673774;
    double VolumeModifierPulmVeinR = 0.489350;
    double VolumeModifierSmallIntestine = 1.159424;
    double VolumeModifierSplanchnic = 1.159424;
    double VolumeModifierSpleen = 1.159424;
    double VolumeModifierTorsoBone = 1.159737;
    double VolumeModifierTorsoFat = 1.159737;
    double VolumeModifierTorsoMuscle = 1.287308;
    double VolumeModifierTorsoSkin = 1.043262;
    double VolumeModifierVenaCava = 0.959748;//0.864810;

    //And also modify the compliances
    Aorta1ToAorta4.GetComplianceBaseline().SetValue(largeArteriesComplianceModifier * Aorta1ToAorta4.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    Aorta1ToAorta4.GetComplianceBaseline().SetValue(0.96, VolumePerPressureUnit::mL_Per_mmHg);

    //Torso1ToGround.GetComplianceBaseline().SetValue(0.01, VolumePerPressureUnit::mL_Per_mmHg);

    //For Internal Hemorrhage
    AbdominalCavityToGround.GetComplianceBaseline().SetValue(100.0, VolumePerPressureUnit::mL_Per_mmHg);

    RightPulmonaryArteries1.GetVolumeBaseline().SetValue(VolumeModifierPulmArtR * RightPulmonaryArteries1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftPulmonaryArteries1.GetVolumeBaseline().SetValue(VolumeModifierPulmArtL * LeftPulmonaryArteries1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightPulmonaryCapillaries1.GetVolumeBaseline().SetValue(VolumeModifierPulmCapR * RightPulmonaryCapillaries1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftPulmonaryCapillaries1.GetVolumeBaseline().SetValue(VolumeModifierPulmCapL * LeftPulmonaryCapillaries1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightPulmonaryVeins1.GetVolumeBaseline().SetValue(VolumeModifierPulmVeinR * RightPulmonaryVeins1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftPulmonaryVeins1.GetVolumeBaseline().SetValue(VolumeModifierPulmVeinL * LeftPulmonaryVeins1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Aorta1.GetVolumeBaseline().SetValue(VolumeModifierAorta * Aorta1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Extracranial2.GetVolumeBaseline().SetValue(VolumeModifierHead * Extracranial2.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Intracranial2.GetVolumeBaseline().SetValue(VolumeModifierHead * Intracranial2.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LargeIntestine1.GetVolumeBaseline().SetValue(VolumeModifierLargeIntestine * LargeIntestine1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Liver1.GetVolumeBaseline().SetValue(VolumeModifierLiver * Liver1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftArmBone1.GetVolumeBaseline().SetValue(VolumeModifierArm * LeftArmBone1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftArmFat1.GetVolumeBaseline().SetValue(VolumeModifierArm * LeftArmFat1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftArmMicrovasculature1.GetVolumeBaseline().SetValue(VolumeModifierArm * LeftArmMicrovasculature1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftArmMuscle1.GetVolumeBaseline().SetValue(VolumeModifierArmMuscle * LeftArmMuscle1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftArmSkin1.GetVolumeBaseline().SetValue(VolumeModifierArm * LeftArmSkin1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftKidney2.GetVolumeBaseline().SetValue(VolumeModifierKidney * LeftKidney2.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftLegBone1.GetVolumeBaseline().SetValue(VolumeModifierLeg * LeftLegBone1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftLegFat1.GetVolumeBaseline().SetValue(VolumeModifierLeg * LeftLegFat1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftLegMicrovasculature1.GetVolumeBaseline().SetValue(VolumeModifierLeg * LeftLegMicrovasculature1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftLegMuscle1.GetVolumeBaseline().SetValue(VolumeModifierLegMuscle * LeftLegMuscle1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftLegSkin1.GetVolumeBaseline().SetValue(VolumeModifierLeg * LeftLegSkin1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Myocardium2.GetVolumeBaseline().SetValue(VolumeModifierMyocard * Myocardium2.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightArmBone1.GetVolumeBaseline().SetValue(VolumeModifierArm * RightArmBone1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightArmFat1.GetVolumeBaseline().SetValue(VolumeModifierArm * RightArmFat1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightArmMicrovasculature1.GetVolumeBaseline().SetValue(VolumeModifierArm * RightArmMicrovasculature1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightArmMuscle1.GetVolumeBaseline().SetValue(VolumeModifierArmMuscle * RightArmMuscle1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightArmSkin1.GetVolumeBaseline().SetValue(VolumeModifierArm * RightArmSkin1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightKidney2.GetVolumeBaseline().SetValue(VolumeModifierKidney * RightKidney2.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightLegBone1.GetVolumeBaseline().SetValue(VolumeModifierLeg * RightLegBone1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightLegFat1.GetVolumeBaseline().SetValue(VolumeModifierLeg * RightLegFat1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightLegMicrovasculature1.GetVolumeBaseline().SetValue(VolumeModifierLeg * RightLegMicrovasculature1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightLegMuscle1.GetVolumeBaseline().SetValue(VolumeModifierLegMuscle * RightLegMuscle1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightLegSkin1.GetVolumeBaseline().SetValue(VolumeModifierLeg * RightLegSkin1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    SmallIntestine1.GetVolumeBaseline().SetValue(VolumeModifierSmallIntestine * SmallIntestine1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Splanchnic1.GetVolumeBaseline().SetValue(VolumeModifierSplanchnic * Splanchnic1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Spleen1.GetVolumeBaseline().SetValue(VolumeModifierSpleen * Spleen1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    TorsoBone1.GetVolumeBaseline().SetValue(VolumeModifierTorsoBone * TorsoBone1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    TorsoFat1.GetVolumeBaseline().SetValue(VolumeModifierTorsoFat * TorsoFat1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    TorsoMuscle1.GetVolumeBaseline().SetValue(VolumeModifierTorsoMuscle * TorsoMuscle1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    TorsoSkin1.GetVolumeBaseline().SetValue(VolumeModifierTorsoSkin * TorsoSkin1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    VenaCava1.GetVolumeBaseline().SetValue(VolumeModifierVenaCava * VenaCava1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);

    // Prepare circuit for compartment creation
    cCardiovascular.SetNextAndCurrentFromBaselines();
    cCardiovascular.StateChange();

    SEFluidCircuit& cCombinedCardiovascular = m_Circuits->GetActiveCardiovascularCircuit();
    cCombinedCardiovascular.AddCircuit(cCardiovascular);
    cCombinedCardiovascular.SetNextAndCurrentFromBaselines();
    cCombinedCardiovascular.StateChange();

    /////////////////////////
    // Create Compartments //
    /////////////////////////

    /////////////////
    // Right Heart //
    SELiquidCompartment& vRightHeart = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightHeart);
    vRightHeart.MapNode(RightHeart1);
    vRightHeart.MapNode(RightHeart2);
    vRightHeart.MapNode(RightHeart3);
    vRightHeart.MapNode(MainPulmonaryArteries1);
    //////////////////////////////
    // Right Pulmonary Arteries //
    SELiquidCompartment& vRightPulmonaryArteries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightPulmonaryArteries);
    vRightPulmonaryArteries.MapNode(RightPulmonaryArteries1);
    vRightPulmonaryArteries.MapNode(RightIntermediatePulmonaryArteries1);
    /////////////////////////////
    // Left Pulmonary Arteries //
    SELiquidCompartment& vLeftPulmonaryArteries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftPulmonaryArteries);
    vLeftPulmonaryArteries.MapNode(LeftPulmonaryArteries1);
    vLeftPulmonaryArteries.MapNode(LeftIntermediatePulmonaryArteries1);
    ////////////////////////
    // Pulmonary Arteries //
    SELiquidCompartment& vPulmonaryArteries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::PulmonaryArteries);
    vPulmonaryArteries.AddChild(vRightPulmonaryArteries);
    vPulmonaryArteries.AddChild(vLeftPulmonaryArteries);
    /////////////////////////////////
    // Right Pulmonary Capillaries //
    SELiquidCompartment& vRightPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightPulmonaryCapillaries);
    vRightPulmonaryCapillaries.MapNode(RightPulmonaryCapillaries1);
    ////////////////////////////////
    // Left Pulmonary Capillaries //
    SELiquidCompartment& vLeftPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftPulmonaryCapillaries);
    vLeftPulmonaryCapillaries.MapNode(LeftPulmonaryCapillaries1);
    ///////////////////////////
    // Pulmonary Capillaries //
    SELiquidCompartment& vPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::PulmonaryCapillaries);
    vPulmonaryCapillaries.AddChild(vRightPulmonaryCapillaries);
    vPulmonaryCapillaries.AddChild(vLeftPulmonaryCapillaries);
    ///////////////////////////
    // Right Pulmonary Veins //
    SELiquidCompartment& vRightPulmonaryVeins = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightPulmonaryVeins);
    vRightPulmonaryVeins.MapNode(RightPulmonaryVeins1);
    //////////////////////////
    // Left Pulmonary Veins //
    SELiquidCompartment& vLeftPulmonaryVeins = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftPulmonaryVeins);
    vLeftPulmonaryVeins.MapNode(LeftPulmonaryVeins1);
    /////////////////////
    // Pulmonary Veins //
    SELiquidCompartment& vPulmonaryVeins = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::PulmonaryVeins);
    vPulmonaryVeins.AddChild(vRightPulmonaryVeins);
    vPulmonaryVeins.AddChild(vLeftPulmonaryVeins);
    ////////////////
    // Left Heart //
    SELiquidCompartment& vLeftHeart = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftHeart);
    vLeftHeart.MapNode(LeftHeart1);
    vLeftHeart.MapNode(LeftHeart2);
    vLeftHeart.MapNode(LeftHeart3);
    ///////////
    // Aorta //
    SELiquidCompartment& vAorta = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Aorta);
    vAorta.MapNode(Aorta1);
    vAorta.MapNode(Aorta2);
    vAorta.MapNode(Aorta3);
    vAorta.MapNode(Aorta4);
    SELiquidCompartment& vArterialBuffer = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::ArterialBuffer);
    vArterialBuffer.MapNode(Aorta5);

    ///////////
    // Abdominal Cavity //
    SELiquidCompartment& vAbdominalCavity = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::AbdominalCavity);
    vAbdominalCavity.MapNode(AbdominalCavity);
    // Head //
    SELiquidCompartment& vExtracranial = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Extracranial);
    vExtracranial.MapNode(Extracranial1);
    vExtracranial.MapNode(Extracranial2);
    vExtracranial.MapNode(Extracranial3);
    SELiquidCompartment& vIntracranial = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Intracranial);
    vIntracranial.MapNode(Intracranial1);
    vIntracranial.MapNode(Intracranial2);
    vIntracranial.MapNode(Intracranial3);
    /////////////////////
    // Large Intestine //
    SELiquidCompartment& vLargeIntestine = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LargeIntestine);
    vLargeIntestine.MapNode(LargeIntestine1);
    ///////////
    // Liver //
    SELiquidCompartment& vLiver = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Liver);
    vLiver.MapNode(Liver1);
    vLiver.MapNode(Liver2);
    vLiver.MapNode(PortalVein1);
    //////////////
    // Left Arm //
    SELiquidCompartment& vLeftArmArterioles = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmArterioles);
    vLeftArmArterioles.MapNode(LeftArm1);
    SELiquidCompartment& vLeftArmBone = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmBone);
    vLeftArmBone.MapNode(LeftArmBone1);
    SELiquidCompartment& vLeftArmFat = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmFat);
    vLeftArmFat.MapNode(LeftArmFat1);
    SELiquidCompartment& vLeftArmMicrovasculature = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmMicrovasculature);
    vLeftArmMicrovasculature.MapNode(LeftArmMicrovasculature1);
    SELiquidCompartment& vLeftArmMuscle = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmMuscle);
    vLeftArmMuscle.MapNode(LeftArmMuscle1);
    SELiquidCompartment& vLeftArmSkin = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmSkin);
    vLeftArmSkin.MapNode(LeftArmSkin1);
    SELiquidCompartment& vLeftArmVenules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmVenules);
    vLeftArmVenules.MapNode(LeftArm2);
    /////////////////
    // Left Kidney //
    SELiquidCompartment& vLeftKidney = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftKidney);
    vLeftKidney.MapNode(LeftKidney1);
    vLeftKidney.MapNode(LeftKidney2);
    vLeftKidney.MapNode(LeftKidney3);
    //////////////
    // Left Leg //
    SELiquidCompartment& vLeftLegArterioles = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegArterioles);
    vLeftLegArterioles.MapNode(LeftLeg1);
    SELiquidCompartment& vLeftLegBone = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegBone);
    vLeftLegBone.MapNode(LeftLegBone1);
    SELiquidCompartment& vLeftLegFat = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegFat);
    vLeftLegFat.MapNode(LeftLegFat1);
    SELiquidCompartment& vLeftLegMicrovasculature = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegMicrovasculature);
    vLeftLegMicrovasculature.MapNode(LeftLegMicrovasculature1);
    SELiquidCompartment& vLeftLegMuscle = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegMuscle);
    vLeftLegMuscle.MapNode(LeftLegMuscle1);
    SELiquidCompartment& vLeftLegSkin = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegSkin);
    vLeftLegSkin.MapNode(LeftLegSkin1);
    SELiquidCompartment& vLeftLegVenules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegVenules);
    vLeftLegVenules.MapNode(LeftLeg2);
    ////////////////
    // Myocardium //
    SELiquidCompartment& vMyocardium = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Myocardium);
    vMyocardium.MapNode(Myocardium1);
    vMyocardium.MapNode(Myocardium2);
    vMyocardium.MapNode(Myocardium3);
    /////////////////
    // Pericardium //
    SELiquidCompartment& vPericardium = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Pericardium);
    vPericardium.MapNode(Pericardium);
    ///////////////
    // Right Arm //
    SELiquidCompartment& vRightArmArterioles = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmArterioles);
    vRightArmArterioles.MapNode(RightArm1);
    SELiquidCompartment& vRightArmBone = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmBone);
    vRightArmBone.MapNode(RightArmBone1);
    SELiquidCompartment& vRightArmFat = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmFat);
    vRightArmFat.MapNode(RightArmFat1);
    SELiquidCompartment& vRightArmMicrovasculature = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmMicrovasculature);
    vRightArmMicrovasculature.MapNode(RightArmMicrovasculature1);
    SELiquidCompartment& vRightArmMuscle = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmMuscle);
    vRightArmMuscle.MapNode(RightArmMuscle1);
    SELiquidCompartment& vRightArmSkin = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmSkin);
    vRightArmSkin.MapNode(RightArmSkin1);
    SELiquidCompartment& vRightArmVenules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmVenules);
    vRightArmVenules.MapNode(RightArm2);
    //////////////////
    // Right Kidney // 
    SELiquidCompartment& vRightKidney = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightKidney);
    vRightKidney.MapNode(RightKidney1);
    vRightKidney.MapNode(RightKidney2);
    vRightKidney.MapNode(RightKidney3);
    ///////////////
    // Right Leg //
    SELiquidCompartment& vRightLegArterioles = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegArterioles);
    vRightLegArterioles.MapNode(RightLeg1);
    SELiquidCompartment& vRightLegBone = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegBone);
    vRightLegBone.MapNode(RightLegBone1);
    SELiquidCompartment& vRightLegFat = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegFat);
    vRightLegFat.MapNode(RightLegFat1);
    SELiquidCompartment& vRightLegMicrovasculature = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegMicrovasculature);
    vRightLegMicrovasculature.MapNode(RightLegMicrovasculature1);
    SELiquidCompartment& vRightLegMuscle = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegMuscle);
    vRightLegMuscle.MapNode(RightLegMuscle1);
    SELiquidCompartment& vRightLegSkin = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegSkin);
    vRightLegSkin.MapNode(RightLegSkin1);
    SELiquidCompartment& vRightLegVenules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegVenules);
    vRightLegVenules.MapNode(RightLeg2);
    /////////////////////
    // Gut1 //
    SELiquidCompartment& vGut1 = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Gut1);
    vGut1.MapNode(Gut1);
    /////////////////////
    // Small Intestine //
    SELiquidCompartment& vSmallIntestine = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::SmallIntestine);
    vSmallIntestine.MapNode(SmallIntestine1);
    ////////////////
    // Splanchnic //
    SELiquidCompartment& vSplanchnic = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Splanchnic);
    vSplanchnic.MapNode(Splanchnic1);
    ////////////
    // Spleen //
    SELiquidCompartment& vSpleen = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Spleen);
    vSpleen.MapNode(Spleen1);
    ///////////
    // Torso //
    SELiquidCompartment& vTorsoArterioles = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoArterioles);
    vTorsoArterioles.MapNode(Torso1);
    SELiquidCompartment& vTorsoBone = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoBone);
    vTorsoBone.MapNode(TorsoBone1);
    SELiquidCompartment& vTorsoFat = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoFat);
    vTorsoFat.MapNode(TorsoFat1);
    SELiquidCompartment& vTorsoMicrovasculature = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoMicrovasculature);
    vTorsoMicrovasculature.MapNode(TorsoMicrovasculature1);
    SELiquidCompartment& vTorsoMuscle = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoMuscle);
    vTorsoMuscle.MapNode(TorsoMuscle1);
    SELiquidCompartment& vTorsoSkin = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoSkin);
    vTorsoSkin.MapNode(TorsoSkin1);
    SELiquidCompartment& vTorsoVenules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoVenules);
    vTorsoVenules.MapNode(Torso2);
    //////////////
    // VenaCava //
    SELiquidCompartment& vVenaCava = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::VenaCava);
    vVenaCava.MapNode(VenaCava1);
    SELiquidCompartment& vVenousBuffer = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::VenousBuffer);
    vVenousBuffer.MapNode(VenaCava2);
    ////////////
    // Ground //
    SELiquidCompartment& vGround = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Ground);
    vGround.MapNode(Ground);

    //////////////////////////
    // Set up our hierarchy //
    //////////////////////////
    SELiquidCompartment& vHead = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Head);
    vHead.AddChild(vExtracranial);
    vHead.AddChild(vIntracranial);
    SELiquidCompartment& vKidneys = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Kidneys);
    vKidneys.AddChild(vLeftKidney);
    vKidneys.AddChild(vRightKidney);
    SELiquidCompartment& vHeart = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Heart);
    vHeart.AddChild(vMyocardium);
    vHeart.AddChild(vLeftHeart);
    vHeart.AddChild(vRightHeart);
    vHeart.AddChild(vPericardium);
    SELiquidCompartment& vLeftLung = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLung);
    vLeftLung.AddChild(vLeftPulmonaryArteries);
    vLeftLung.AddChild(vLeftPulmonaryCapillaries);
    vLeftLung.AddChild(vLeftPulmonaryVeins);
    SELiquidCompartment& vRightLung = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLung);
    vRightLung.AddChild(vRightPulmonaryArteries);
    vRightLung.AddChild(vRightPulmonaryCapillaries);
    vRightLung.AddChild(vRightPulmonaryVeins);
    SELiquidCompartment& vLungs = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Lungs);
    vLungs.AddChild(vLeftLung);
    vLungs.AddChild(vRightLung);
    SELiquidCompartment& vLeftArm = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArm);
    vLeftArm.AddChild(vLeftArmArterioles);
    vLeftArm.AddChild(vLeftArmBone);
    vLeftArm.AddChild(vLeftArmFat);
    vLeftArm.AddChild(vLeftArmMicrovasculature);
    vLeftArm.AddChild(vLeftArmMuscle);
    vLeftArm.AddChild(vLeftArmSkin);
    vLeftArm.AddChild(vLeftArmVenules);
    SELiquidCompartment& vLeftLeg = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLeg);
    vLeftLeg.AddChild(vLeftLegArterioles);
    vLeftLeg.AddChild(vLeftLegBone);
    vLeftLeg.AddChild(vLeftLegFat);
    vLeftLeg.AddChild(vLeftLegMicrovasculature);
    vLeftLeg.AddChild(vLeftLegMuscle);
    vLeftLeg.AddChild(vLeftLegSkin);
    vLeftLeg.AddChild(vLeftLegVenules);
    SELiquidCompartment& vRightArm = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightArm);
    vRightArm.AddChild(vRightArmArterioles);
    vRightArm.AddChild(vRightArmBone);
    vRightArm.AddChild(vRightArmFat);
    vRightArm.AddChild(vRightArmMicrovasculature);
    vRightArm.AddChild(vRightArmMuscle);
    vRightArm.AddChild(vRightArmSkin);
    vRightArm.AddChild(vRightArmVenules);
    SELiquidCompartment& vRightLeg = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightLeg);
    vRightLeg.AddChild(vRightLegArterioles);
    vRightLeg.AddChild(vRightLegBone);
    vRightLeg.AddChild(vRightLegFat);
    vRightLeg.AddChild(vRightLegMicrovasculature);
    vRightLeg.AddChild(vRightLegMuscle);
    vRightLeg.AddChild(vRightLegSkin);
    vRightLeg.AddChild(vRightLegVenules);
    SELiquidCompartment& vTorso = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Torso);
    vTorso.AddChild(vTorsoArterioles);
    vTorso.AddChild(vTorsoBone);
    vTorso.AddChild(vTorsoFat);
    vTorso.AddChild(vTorsoMicrovasculature);
    vTorso.AddChild(vTorsoMuscle);
    vTorso.AddChild(vTorsoSkin);
    vTorso.AddChild(vTorsoVenules);
    SELiquidCompartment& vGut = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Gut);
    vGut.AddChild(vSplanchnic);
    vGut.AddChild(vSmallIntestine);
    vGut.AddChild(vLargeIntestine);
    vGut.AddChild(vGut1);
    SELiquidCompartment& vAbdomen = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Abdomen);
    vAbdomen.AddChild(vSplanchnic);
    vAbdomen.AddChild(vSmallIntestine);
    vAbdomen.AddChild(vLargeIntestine);
    vAbdomen.AddChild(vLeftKidney);
    vAbdomen.AddChild(vRightKidney);
    vAbdomen.AddChild(vSpleen);
    vAbdomen.AddChild(vLiver);
    vAbdomen.AddChild(vAbdominalCavity);
    SELiquidCompartment& vBone = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Bone);
    vBone.AddChild(vLeftArmBone);
    vBone.AddChild(vLeftLegBone);
    vBone.AddChild(vRightArmBone);
    vBone.AddChild(vRightLegBone);
    vBone.AddChild(vTorsoBone);
    SELiquidCompartment& vFat = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Fat);
    vFat.AddChild(vLeftArmFat);
    vFat.AddChild(vLeftLegFat);
    vFat.AddChild(vRightArmFat);
    vFat.AddChild(vRightLegFat);
    vFat.AddChild(vTorsoFat);
    SELiquidCompartment& vMuscle = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Muscle);
    vMuscle.AddChild(vLeftArmMuscle);
    vMuscle.AddChild(vLeftLegMuscle);
    vMuscle.AddChild(vRightArmMuscle);
    vMuscle.AddChild(vRightLegMuscle);
    vMuscle.AddChild(vTorsoMuscle);
    SELiquidCompartment& vSkin = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::Skin);
    vSkin.AddChild(vLeftArmSkin);
    vSkin.AddChild(vLeftLegSkin);
    vSkin.AddChild(vRightArmSkin);
    vSkin.AddChild(vRightLegSkin);
    vSkin.AddChild(vTorsoSkin);

    //////////////////
    // Create Links //
    //////////////////

    /////////////////////
    // Heart and Lungs //
    SELiquidCompartmentLink& vVenousBufferToVenaCava = m_Compartments->CreateLiquidLink(vVenousBuffer, vVenaCava, pulse::ExpandedVascularLink::VenousBufferToVenaCava);
    vVenousBufferToVenaCava.MapPath(VenaCava2ToVenaCava1);
    SELiquidCompartmentLink& vVenaCavaToRightHeart = m_Compartments->CreateLiquidLink(vVenaCava, vRightHeart, pulse::ExpandedVascularLink::VenaCavaToRightHeart);
    vVenaCavaToRightHeart.MapPath(VenaCava1ToRightHeart2);
    SELiquidCompartmentLink& vRightHeartToLeftPulmonaryArteries = m_Compartments->CreateLiquidLink(vRightHeart, vLeftPulmonaryArteries, pulse::ExpandedVascularLink::RightHeartToLeftPulmonaryArteries);
    vRightHeartToLeftPulmonaryArteries.MapPath(MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryCapillaries, pulse::ExpandedVascularLink::LeftPulmonaryArteriesToCapillaries);
    vLeftPulmonaryArteriesToCapillaries.MapPath(LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToVeins = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedVascularLink::LeftPulmonaryArteriesToVeins);
    vLeftPulmonaryArteriesToVeins.MapPath(LeftPulmonaryArteries1ToLeftPulmonaryVeins1);
    SELiquidCompartmentLink& vLeftPulmonaryCapillariesToVeins = m_Compartments->CreateLiquidLink(vLeftPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedVascularLink::LeftPulmonaryCapillariesToVeins);
    vLeftPulmonaryCapillariesToVeins.MapPath(LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1);
    SELiquidCompartmentLink& vLeftPulmonaryVeinsToLeftHeart = m_Compartments->CreateLiquidLink(vLeftPulmonaryVeins, vLeftHeart, pulse::ExpandedVascularLink::LeftPulmonaryVeinsToLeftHeart);
    vLeftPulmonaryVeinsToLeftHeart.MapPath(LeftIntermediatePulmonaryVeins1ToLeftHeart2);
    SELiquidCompartmentLink& vRightHeartToRightPulmonaryArteries = m_Compartments->CreateLiquidLink(vRightHeart, vRightPulmonaryArteries, pulse::ExpandedVascularLink::RightHeartToRightPulmonaryArteries);
    vRightHeartToRightPulmonaryArteries.MapPath(MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightPulmonaryCapillaries, pulse::ExpandedVascularLink::RightPulmonaryArteriesToCapillaries);
    vRightPulmonaryArteriesToCapillaries.MapPath(RightPulmonaryArteries1ToRightPulmonaryCapillaries1);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToVeins = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightPulmonaryVeins, pulse::ExpandedVascularLink::RightPulmonaryArteriesToVeins);
    vRightPulmonaryArteriesToVeins.MapPath(RightPulmonaryArteries1ToRightPulmonaryVeins1);
    SELiquidCompartmentLink& vRightPulmonaryCapillariesToVeins = m_Compartments->CreateLiquidLink(vRightPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedVascularLink::RightPulmonaryCapillariesToVeins);
    vRightPulmonaryCapillariesToVeins.MapPath(RightPulmonaryCapillaries1ToRightPulmonaryVeins1);
    SELiquidCompartmentLink& vRightPulmonaryVeinsToLeftHeart = m_Compartments->CreateLiquidLink(vRightPulmonaryVeins, vLeftHeart, pulse::ExpandedVascularLink::RightPulmonaryVeinsToLeftHeart);
    vRightPulmonaryVeinsToLeftHeart.MapPath(RightIntermediatePulmonaryVeins1ToLeftHeart2);
    SELiquidCompartmentLink& vLeftHeartToAorta = m_Compartments->CreateLiquidLink(vLeftHeart, vAorta, pulse::ExpandedVascularLink::LeftHeartToAorta);
    vLeftHeartToAorta.MapPath(LeftHeart1ToAorta2);
    SELiquidCompartmentLink& vAortaToArterialBuffer = m_Compartments->CreateLiquidLink(vAorta, vArterialBuffer, pulse::ExpandedVascularLink::AortaToArterialBuffer);
    vAortaToArterialBuffer.MapPath(Aorta1ToAorta5);
    ///////////
    // Head //
    SELiquidCompartmentLink& vAortaToExtracranial = m_Compartments->CreateLiquidLink(vArterialBuffer, vExtracranial, pulse::ExpandedVascularLink::AortaToExtracranial);
    vAortaToExtracranial.MapPath(Aorta5ToExtracranial1);
    SELiquidCompartmentLink& vExtracranialToVenaCava = m_Compartments->CreateLiquidLink(vExtracranial, vVenousBuffer, pulse::ExpandedVascularLink::ExtracranialToVenaCava);
    vExtracranialToVenaCava.MapPath(Extracranial3ToVenaCava2);
    SELiquidCompartmentLink& vAortaToIntracranial = m_Compartments->CreateLiquidLink(vArterialBuffer, vIntracranial, pulse::ExpandedVascularLink::AortaToIntracranial);
    vAortaToIntracranial.MapPath(Aorta5ToIntracranial1);
    SELiquidCompartmentLink& vIntracranialToVenaCava = m_Compartments->CreateLiquidLink(vIntracranial, vVenousBuffer, pulse::ExpandedVascularLink::IntracranialToVenaCava);
    vIntracranialToVenaCava.MapPath(Intracranial3ToVenaCava2);
    /////////////////////
    // Gut //
    SELiquidCompartmentLink& vAortaToGut = m_Compartments->CreateLiquidLink(vArterialBuffer, vGut1, pulse::ExpandedVascularLink::AortaToGut);
    vAortaToGut.MapPath(Aorta5ToGut1);
    /////////////////////
    // Large Intestine //
    SELiquidCompartmentLink& vAortaToLargeIntestine = m_Compartments->CreateLiquidLink(vGut1, vLargeIntestine, pulse::ExpandedVascularLink::AortaToLargeIntestine);
    vAortaToLargeIntestine.MapPath(Gut1ToLargeIntestine1);
    SELiquidCompartmentLink& vLargeIntestineToLiver = m_Compartments->CreateLiquidLink(vLargeIntestine, vLiver, pulse::ExpandedVascularLink::LargeIntestineToLiver);
    vLargeIntestineToLiver.MapPath(LargeIntestine1ToPortalVein1);
    ///////////
    // Liver //
    SELiquidCompartmentLink& vAortaToLiver = m_Compartments->CreateLiquidLink(vGut1, vLiver, pulse::ExpandedVascularLink::AortaToLiver);
    vAortaToLiver.MapPath(Gut1ToLiver1);
    SELiquidCompartmentLink& vLiverToVenaCava = m_Compartments->CreateLiquidLink(vLiver, vVenousBuffer, pulse::ExpandedVascularLink::LiverToVenaCava);
    vLiverToVenaCava.MapPath(Liver2ToVenaCava2);
    //////////////
    // Left Arm //
    SELiquidCompartmentLink& vAortaToLeftArmArterioles = m_Compartments->CreateLiquidLink(vArterialBuffer, vLeftArmArterioles, pulse::ExpandedVascularLink::AortaToLeftArmArterioles);
    SELiquidCompartmentLink& vLeftArmArteriolesToLeftArmBone = m_Compartments->CreateLiquidLink(vLeftArmArterioles, vLeftArmBone, pulse::ExpandedVascularLink::LeftArmArteriolesToLeftArmBone);
    SELiquidCompartmentLink& vLeftArmArteriolesToLeftArmFat = m_Compartments->CreateLiquidLink(vLeftArmArterioles, vLeftArmFat, pulse::ExpandedVascularLink::LeftArmArteriolesToLeftArmFat);
    SELiquidCompartmentLink& vLeftArmArteriolesToLeftArmMicrovasculature = m_Compartments->CreateLiquidLink(vLeftArmArterioles, vLeftArmMicrovasculature, pulse::ExpandedVascularLink::LeftArmArteriolesToLeftArmMicrovasculature);
    SELiquidCompartmentLink& vLeftArmArteriolesToLeftArmMuscle = m_Compartments->CreateLiquidLink(vLeftArmArterioles, vLeftArmMuscle, pulse::ExpandedVascularLink::LeftArmArteriolesToLeftArmMuscle);
    SELiquidCompartmentLink& vLeftArmArteriolesToLeftArmSkin = m_Compartments->CreateLiquidLink(vLeftArmArterioles, vLeftArmSkin, pulse::ExpandedVascularLink::LeftArmArteriolesToLeftArmSkin);
    SELiquidCompartmentLink& vLeftArmBoneToLeftArmVenules = m_Compartments->CreateLiquidLink(vLeftArmBone, vLeftArmVenules, pulse::ExpandedVascularLink::LeftArmBoneToLeftArmVenules);
    SELiquidCompartmentLink& vLeftArmFatToLeftArmVenules = m_Compartments->CreateLiquidLink(vLeftArmFat, vLeftArmVenules, pulse::ExpandedVascularLink::LeftArmFatToLeftArmVenules);
    SELiquidCompartmentLink& vLeftArmMicrovasculatureToLeftArmVenules = m_Compartments->CreateLiquidLink(vLeftArmMicrovasculature, vLeftArmVenules, pulse::ExpandedVascularLink::LeftArmMicrovasculatureToLeftArmVenules);
    SELiquidCompartmentLink& vLeftArmMuscleToLeftArmVenules = m_Compartments->CreateLiquidLink(vLeftArmMuscle, vLeftArmVenules, pulse::ExpandedVascularLink::LeftArmMuscleToLeftArmVenules);
    SELiquidCompartmentLink& vLeftArmSkinToLeftArmVenules = m_Compartments->CreateLiquidLink(vLeftArmSkin, vLeftArmVenules, pulse::ExpandedVascularLink::LeftArmSkinToLeftArmVenules);
    SELiquidCompartmentLink& vLeftArmVenulesToVenaCava = m_Compartments->CreateLiquidLink(vLeftArmVenules, vVenousBuffer, pulse::ExpandedVascularLink::LeftArmVenulesToVenaCava);
    vAortaToLeftArmArterioles.MapPath(Aorta5ToLeftArm1);
    vLeftArmArteriolesToLeftArmBone.MapPath(LeftArm1ToLeftArmBone1);
    vLeftArmArteriolesToLeftArmFat.MapPath(LeftArm1ToLeftArmFat1);
    vLeftArmArteriolesToLeftArmMicrovasculature.MapPath(LeftArm1ToLeftArmMicrovasculature1);
    vLeftArmArteriolesToLeftArmMuscle.MapPath(LeftArm1ToLeftArmMuscle1);
    vLeftArmArteriolesToLeftArmSkin.MapPath(LeftArm1ToLeftArmSkin1);
    vLeftArmBoneToLeftArmVenules.MapPath(LeftArmBone1ToLeftArm2);
    vLeftArmFatToLeftArmVenules.MapPath(LeftArmFat1ToLeftArm2);
    vLeftArmMicrovasculatureToLeftArmVenules.MapPath(LeftArmMicrovasculature1ToLeftArm2);
    vLeftArmMuscleToLeftArmVenules.MapPath(LeftArmMuscle1ToLeftArm2);
    vLeftArmSkinToLeftArmVenules.MapPath(LeftArmSkin1ToLeftArm2);
    vLeftArmVenulesToVenaCava.MapPath(LeftArm2ToVenaCava2);

    /////////////////
    // Left Kidney //
    SELiquidCompartmentLink& vAortaToLeftKidney = m_Compartments->CreateLiquidLink(vArterialBuffer, vLeftKidney, pulse::ExpandedVascularLink::AortaToLeftKidney);
    vAortaToLeftKidney.MapPath(Aorta5ToLeftKidney1);
    SELiquidCompartmentLink& vLeftKidneyToVenaCava = m_Compartments->CreateLiquidLink(vLeftKidney, vVenousBuffer, pulse::ExpandedVascularLink::LeftKidneyToVenaCava);
    vLeftKidneyToVenaCava.MapPath(LeftKidney3ToVenaCava2);
    //////////////
    // Left Leg //
    SELiquidCompartmentLink& vAortaToLeftLegArterioles = m_Compartments->CreateLiquidLink(vArterialBuffer, vLeftLegArterioles, pulse::ExpandedVascularLink::AortaToLeftLegArterioles);
    SELiquidCompartmentLink& vLeftLegArteriolesToLeftLegBone = m_Compartments->CreateLiquidLink(vLeftLegArterioles, vLeftLegBone, pulse::ExpandedVascularLink::LeftLegArteriolesToLeftLegBone);
    SELiquidCompartmentLink& vLeftLegArteriolesToLeftLegFat = m_Compartments->CreateLiquidLink(vLeftLegArterioles, vLeftLegFat, pulse::ExpandedVascularLink::LeftLegArteriolesToLeftLegFat);
    SELiquidCompartmentLink& vLeftLegArteriolesToLeftLegMicrovasculature = m_Compartments->CreateLiquidLink(vLeftLegArterioles, vLeftLegMicrovasculature, pulse::ExpandedVascularLink::LeftLegArteriolesToLeftLegMicrovasculature);
    SELiquidCompartmentLink& vLeftLegArteriolesToLeftLegMuscle = m_Compartments->CreateLiquidLink(vLeftLegArterioles, vLeftLegMuscle, pulse::ExpandedVascularLink::LeftLegArteriolesToLeftLegMuscle);
    SELiquidCompartmentLink& vLeftLegArteriolesToLeftLegSkin = m_Compartments->CreateLiquidLink(vLeftLegArterioles, vLeftLegSkin, pulse::ExpandedVascularLink::LeftLegArteriolesToLeftLegSkin);
    SELiquidCompartmentLink& vLeftLegBoneToLeftLegVenules = m_Compartments->CreateLiquidLink(vLeftLegBone, vLeftLegVenules, pulse::ExpandedVascularLink::LeftLegBoneToLeftLegVenules);
    SELiquidCompartmentLink& vLeftLegFatToLeftLegVenules = m_Compartments->CreateLiquidLink(vLeftLegFat, vLeftLegVenules, pulse::ExpandedVascularLink::LeftLegFatToLeftLegVenules);
    SELiquidCompartmentLink& vLeftLegMicrovasculatureToLeftLegVenules = m_Compartments->CreateLiquidLink(vLeftLegMicrovasculature, vLeftLegVenules, pulse::ExpandedVascularLink::LeftLegMicrovasculatureToLeftLegVenules);
    SELiquidCompartmentLink& vLeftLegMuscleToLeftLegVenules = m_Compartments->CreateLiquidLink(vLeftLegMuscle, vLeftLegVenules, pulse::ExpandedVascularLink::LeftLegMuscleToLeftLegVenules);
    SELiquidCompartmentLink& vLeftLegSkinToLeftLegVenules = m_Compartments->CreateLiquidLink(vLeftLegSkin, vLeftLegVenules, pulse::ExpandedVascularLink::LeftLegSkinToLeftLegVenules);
    SELiquidCompartmentLink& vLeftLegVenulesToVenaCava = m_Compartments->CreateLiquidLink(vLeftLegVenules, vVenousBuffer, pulse::ExpandedVascularLink::LeftLegVenulesToVenaCava);
    vAortaToLeftLegArterioles.MapPath(Aorta5ToLeftLeg1);
    vLeftLegArteriolesToLeftLegBone.MapPath(LeftLeg1ToLeftLegBone1);
    vLeftLegArteriolesToLeftLegFat.MapPath(LeftLeg1ToLeftLegFat1);
    vLeftLegArteriolesToLeftLegMicrovasculature.MapPath(LeftLeg1ToLeftLegMicrovasculature1);
    vLeftLegArteriolesToLeftLegMuscle.MapPath(LeftLeg1ToLeftLegMuscle1);
    vLeftLegArteriolesToLeftLegSkin.MapPath(LeftLeg1ToLeftLegSkin1);
    vLeftLegBoneToLeftLegVenules.MapPath(LeftLegBone1ToLeftLeg2);
    vLeftLegFatToLeftLegVenules.MapPath(LeftLegFat1ToLeftLeg2);
    vLeftLegMicrovasculatureToLeftLegVenules.MapPath(LeftLegMicrovasculature1ToLeftLeg2);
    vLeftLegMuscleToLeftLegVenules.MapPath(LeftLegMuscle1ToLeftLeg2);
    vLeftLegSkinToLeftLegVenules.MapPath(LeftLegSkin1ToLeftLeg2);
    vLeftLegVenulesToVenaCava.MapPath(LeftLeg2ToVenaCava2);

    ////////////////
    // Myocardium //
    SELiquidCompartmentLink& vAortaToMyocardium = m_Compartments->CreateLiquidLink(vArterialBuffer, vMyocardium, pulse::ExpandedVascularLink::AortaToMyocardium);
    vAortaToMyocardium.MapPath(Aorta5ToMyocardium1);
    SELiquidCompartmentLink& vMyocardiumToVenaCava = m_Compartments->CreateLiquidLink(vMyocardium, vVenousBuffer, pulse::ExpandedVascularLink::MyocardiumToVenaCava);
    vMyocardiumToVenaCava.MapPath(Myocardium3ToVenaCava2);
    ///////////////
    // Right Arm //
    SELiquidCompartmentLink& vAortaToRightArmArterioles = m_Compartments->CreateLiquidLink(vArterialBuffer, vRightArmArterioles, pulse::ExpandedVascularLink::AortaToRightArmArterioles);
    SELiquidCompartmentLink& vRightArmArteriolesToRightArmBone = m_Compartments->CreateLiquidLink(vRightArmArterioles, vRightArmBone, pulse::ExpandedVascularLink::RightArmArteriolesToRightArmBone);
    SELiquidCompartmentLink& vRightArmArteriolesToRightArmFat = m_Compartments->CreateLiquidLink(vRightArmArterioles, vRightArmFat, pulse::ExpandedVascularLink::RightArmArteriolesToRightArmFat);
    SELiquidCompartmentLink& vRightArmArteriolesToRightArmMicrovasculature = m_Compartments->CreateLiquidLink(vRightArmArterioles, vRightArmMicrovasculature, pulse::ExpandedVascularLink::RightArmArteriolesToRightArmMicrovasculature);
    SELiquidCompartmentLink& vRightArmArteriolesToRightArmMuscle = m_Compartments->CreateLiquidLink(vRightArmArterioles, vRightArmMuscle, pulse::ExpandedVascularLink::RightArmArteriolesToRightArmMuscle);
    SELiquidCompartmentLink& vRightArmArteriolesToRightArmSkin = m_Compartments->CreateLiquidLink(vRightArmArterioles, vRightArmSkin, pulse::ExpandedVascularLink::RightArmArteriolesToRightArmSkin);
    SELiquidCompartmentLink& vRightArmBoneToRightArmVenules = m_Compartments->CreateLiquidLink(vRightArmBone, vRightArmVenules, pulse::ExpandedVascularLink::RightArmBoneToRightArmVenules);
    SELiquidCompartmentLink& vRightArmFatToRightArmVenules = m_Compartments->CreateLiquidLink(vRightArmFat, vRightArmVenules, pulse::ExpandedVascularLink::RightArmFatToRightArmVenules);
    SELiquidCompartmentLink& vRightArmMicrovasculatureToRightArmVenules = m_Compartments->CreateLiquidLink(vRightArmMicrovasculature, vRightArmVenules, pulse::ExpandedVascularLink::RightArmMicrovasculatureToRightArmVenules);
    SELiquidCompartmentLink& vRightArmMuscleToRightArmVenules = m_Compartments->CreateLiquidLink(vRightArmMuscle, vRightArmVenules, pulse::ExpandedVascularLink::RightArmMuscleToRightArmVenules);
    SELiquidCompartmentLink& vRightArmSkinToRightArmVenules = m_Compartments->CreateLiquidLink(vRightArmSkin, vRightArmVenules, pulse::ExpandedVascularLink::RightArmSkinToRightArmVenules);
    SELiquidCompartmentLink& vRightArmVenulesToVenaCava = m_Compartments->CreateLiquidLink(vRightArmVenules, vVenousBuffer, pulse::ExpandedVascularLink::RightArmVenulesToVenaCava);
    vAortaToRightArmArterioles.MapPath(Aorta5ToRightArm1);
    vRightArmArteriolesToRightArmBone.MapPath(RightArm1ToRightArmBone1);
    vRightArmArteriolesToRightArmFat.MapPath(RightArm1ToRightArmFat1);
    vRightArmArteriolesToRightArmMicrovasculature.MapPath(RightArm1ToRightArmMicrovasculature1);
    vRightArmArteriolesToRightArmMuscle.MapPath(RightArm1ToRightArmMuscle1);
    vRightArmArteriolesToRightArmSkin.MapPath(RightArm1ToRightArmSkin1);
    vRightArmBoneToRightArmVenules.MapPath(RightArmBone1ToRightArm2);
    vRightArmFatToRightArmVenules.MapPath(RightArmFat1ToRightArm2);
    vRightArmMicrovasculatureToRightArmVenules.MapPath(RightArmMicrovasculature1ToRightArm2);
    vRightArmMuscleToRightArmVenules.MapPath(RightArmMuscle1ToRightArm2);
    vRightArmSkinToRightArmVenules.MapPath(RightArmSkin1ToRightArm2);
    vRightArmVenulesToVenaCava.MapPath(RightArm2ToVenaCava2);

    //////////////////
    // Right Kidney // 
    SELiquidCompartmentLink& vAortaToRightKidney = m_Compartments->CreateLiquidLink(vArterialBuffer, vRightKidney, pulse::ExpandedVascularLink::AortaToRightKidney);
    vAortaToRightKidney.MapPath(Aorta5ToRightKidney1);
    SELiquidCompartmentLink& vRightKidneyToVenaCava = m_Compartments->CreateLiquidLink(vRightKidney, vVenousBuffer, pulse::ExpandedVascularLink::RightKidneyToVenaCava);
    vRightKidneyToVenaCava.MapPath(RightKidney3ToVenaCava2);
    ///////////////
    // Right Leg //
    SELiquidCompartmentLink& vAortaToRightLegArterioles = m_Compartments->CreateLiquidLink(vArterialBuffer, vRightLegArterioles, pulse::ExpandedVascularLink::AortaToRightLegArterioles);
    SELiquidCompartmentLink& vRightLegArteriolesToRightLegBone = m_Compartments->CreateLiquidLink(vRightLegArterioles, vRightLegBone, pulse::ExpandedVascularLink::RightLegArteriolesToRightLegBone);
    SELiquidCompartmentLink& vRightLegArteriolesToRightLegFat = m_Compartments->CreateLiquidLink(vRightLegArterioles, vRightLegFat, pulse::ExpandedVascularLink::RightLegArteriolesToRightLegFat);
    SELiquidCompartmentLink& vRightLegArteriolesToRightLegMicrovasculature = m_Compartments->CreateLiquidLink(vRightLegArterioles, vRightLegMicrovasculature, pulse::ExpandedVascularLink::RightLegArteriolesToRightLegMicrovasculature);
    SELiquidCompartmentLink& vRightLegArteriolesToRightLegMuscle = m_Compartments->CreateLiquidLink(vRightLegArterioles, vRightLegMuscle, pulse::ExpandedVascularLink::RightLegArteriolesToRightLegMuscle);
    SELiquidCompartmentLink& vRightLegArteriolesToRightLegSkin = m_Compartments->CreateLiquidLink(vRightLegArterioles, vRightLegSkin, pulse::ExpandedVascularLink::RightLegArteriolesToRightLegSkin);
    SELiquidCompartmentLink& vRightLegBoneToRightLegVenules = m_Compartments->CreateLiquidLink(vRightLegBone, vRightLegVenules, pulse::ExpandedVascularLink::RightLegBoneToRightLegVenules);
    SELiquidCompartmentLink& vRightLegFatToRightLegVenules = m_Compartments->CreateLiquidLink(vRightLegFat, vRightLegVenules, pulse::ExpandedVascularLink::RightLegFatToRightLegVenules);
    SELiquidCompartmentLink& vRightLegMicrovasculatureToRightLegVenules = m_Compartments->CreateLiquidLink(vRightLegMicrovasculature, vRightLegVenules, pulse::ExpandedVascularLink::RightLegMicrovasculatureToRightLegVenules);
    SELiquidCompartmentLink& vRightLegMuscleToRightLegVenules = m_Compartments->CreateLiquidLink(vRightLegMuscle, vRightLegVenules, pulse::ExpandedVascularLink::RightLegMuscleToRightLegVenules);
    SELiquidCompartmentLink& vRightLegSkinToRightLegVenules = m_Compartments->CreateLiquidLink(vRightLegSkin, vRightLegVenules, pulse::ExpandedVascularLink::RightLegSkinToRightLegVenules);
    SELiquidCompartmentLink& vRightLegVenulesToVenaCava = m_Compartments->CreateLiquidLink(vRightLegVenules, vVenousBuffer, pulse::ExpandedVascularLink::RightLegVenulesToVenaCava);
    vAortaToRightLegArterioles.MapPath(Aorta5ToRightLeg1);
    vRightLegArteriolesToRightLegBone.MapPath(RightLeg1ToRightLegBone1);
    vRightLegArteriolesToRightLegFat.MapPath(RightLeg1ToRightLegFat1);
    vRightLegArteriolesToRightLegMicrovasculature.MapPath(RightLeg1ToRightLegMicrovasculature1);
    vRightLegArteriolesToRightLegMuscle.MapPath(RightLeg1ToRightLegMuscle1);
    vRightLegArteriolesToRightLegSkin.MapPath(RightLeg1ToRightLegSkin1);
    vRightLegBoneToRightLegVenules.MapPath(RightLegBone1ToRightLeg2);
    vRightLegFatToRightLegVenules.MapPath(RightLegFat1ToRightLeg2);
    vRightLegMicrovasculatureToRightLegVenules.MapPath(RightLegMicrovasculature1ToRightLeg2);
    vRightLegMuscleToRightLegVenules.MapPath(RightLegMuscle1ToRightLeg2);
    vRightLegSkinToRightLegVenules.MapPath(RightLegSkin1ToRightLeg2);
    vRightLegVenulesToVenaCava.MapPath(RightLeg2ToVenaCava2);

    /////////////////////
    // Small Intestine //
    SELiquidCompartmentLink& vAortaToSmallIntestine = m_Compartments->CreateLiquidLink(vGut1, vSmallIntestine, pulse::ExpandedVascularLink::AortaToSmallIntestine);
    vAortaToSmallIntestine.MapPath(Gut1ToSmallIntestine1);
    SELiquidCompartmentLink& vSmallIntestineToLiver = m_Compartments->CreateLiquidLink(vSmallIntestine, vLiver, pulse::ExpandedVascularLink::SmallIntestineToLiver);
    vSmallIntestineToLiver.MapPath(SmallIntestine1ToPortalVein1);
    ////////////////
    // Splanchnic //
    SELiquidCompartmentLink& vAortaToSplanchnic = m_Compartments->CreateLiquidLink(vGut1, vSplanchnic, pulse::ExpandedVascularLink::AortaToSplanchnic);
    vAortaToSplanchnic.MapPath(Gut1ToSplanchnic1);
    SELiquidCompartmentLink& vSplanchnicToLiver = m_Compartments->CreateLiquidLink(vSplanchnic, vLiver, pulse::ExpandedVascularLink::SplanchnicToLiver);
    vSplanchnicToLiver.MapPath(Splanchnic1ToPortalVein1);
    ////////////
    // Spleen //
    SELiquidCompartmentLink& vAortaToSpleen = m_Compartments->CreateLiquidLink(vGut1, vSpleen, pulse::ExpandedVascularLink::AortaToSpleen);
    vAortaToSpleen.MapPath(Gut1ToSpleen1);
    SELiquidCompartmentLink& vSpleenToLiver = m_Compartments->CreateLiquidLink(vSpleen, vLiver, pulse::ExpandedVascularLink::SpleenToLiver);
    vSpleenToLiver.MapPath(Spleen1ToPortalVein1);
    ///////////
    // Torso //
    SELiquidCompartmentLink& vAortaToTorsoArterioles = m_Compartments->CreateLiquidLink(vArterialBuffer, vTorsoArterioles, pulse::ExpandedVascularLink::AortaToTorsoArterioles);
    SELiquidCompartmentLink& vTorsoArteriolesToTorsoBone = m_Compartments->CreateLiquidLink(vTorsoArterioles, vTorsoBone, pulse::ExpandedVascularLink::TorsoArteriolesToTorsoBone);
    SELiquidCompartmentLink& vTorsoArteriolesToTorsoFat = m_Compartments->CreateLiquidLink(vTorsoArterioles, vTorsoFat, pulse::ExpandedVascularLink::TorsoArteriolesToTorsoFat);
    SELiquidCompartmentLink& vTorsoArteriolesToTorsoMicrovasculature = m_Compartments->CreateLiquidLink(vTorsoArterioles, vTorsoMicrovasculature, pulse::ExpandedVascularLink::TorsoArteriolesToTorsoMicrovasculature);
    SELiquidCompartmentLink& vTorsoArteriolesToTorsoMuscle = m_Compartments->CreateLiquidLink(vTorsoArterioles, vTorsoMuscle, pulse::ExpandedVascularLink::TorsoArteriolesToTorsoMuscle);
    SELiquidCompartmentLink& vTorsoArteriolesToTorsoSkin = m_Compartments->CreateLiquidLink(vTorsoArterioles, vTorsoSkin, pulse::ExpandedVascularLink::TorsoArteriolesToTorsoSkin);
    SELiquidCompartmentLink& vTorsoSkinToTorsoVenules = m_Compartments->CreateLiquidLink(vTorsoSkin, vTorsoVenules, pulse::ExpandedVascularLink::TorsoSkinToTorsoVenules);
    SELiquidCompartmentLink& vTorsoMuscleToTorsoVenules = m_Compartments->CreateLiquidLink(vTorsoMuscle, vTorsoVenules, pulse::ExpandedVascularLink::TorsoMuscleToTorsoVenules);
    SELiquidCompartmentLink& vTorsoMicrovasculatureToTorsoVenules = m_Compartments->CreateLiquidLink(vTorsoMicrovasculature, vTorsoVenules, pulse::ExpandedVascularLink::TorsoMicrovasculatureToTorsoVenules);
    SELiquidCompartmentLink& vTorsoFatToTorsoVenules = m_Compartments->CreateLiquidLink(vTorsoFat, vTorsoVenules, pulse::ExpandedVascularLink::TorsoFatToTorsoVenules);
    SELiquidCompartmentLink& vTorsoBoneToTorsoVenules = m_Compartments->CreateLiquidLink(vTorsoBone, vTorsoVenules, pulse::ExpandedVascularLink::TorsoBoneToTorsoVenules);
    SELiquidCompartmentLink& vTorsoVenulesToVenaCava = m_Compartments->CreateLiquidLink(vTorsoVenules, vVenousBuffer, pulse::ExpandedVascularLink::TorsoVenulesToVenaCava);
    vAortaToTorsoArterioles.MapPath(Aorta5ToTorso1);
    vTorsoArteriolesToTorsoBone.MapPath(Torso1ToTorsoBone1);
    vTorsoArteriolesToTorsoFat.MapPath(Torso1ToTorsoFat1);
    vTorsoArteriolesToTorsoMicrovasculature.MapPath(Torso1ToTorsoMicrovasculature1);
    vTorsoArteriolesToTorsoMuscle.MapPath(Torso1ToTorsoMuscle1);
    vTorsoArteriolesToTorsoSkin.MapPath(Torso1ToTorsoSkin1);
    vTorsoSkinToTorsoVenules.MapPath(TorsoSkin1ToTorso2);
    vTorsoMuscleToTorsoVenules.MapPath(TorsoMuscle1ToTorso2);
    vTorsoMicrovasculatureToTorsoVenules.MapPath(TorsoMicrovasculature1ToTorso2);
    vTorsoFatToTorsoVenules.MapPath(TorsoFat1ToTorso2);
    vTorsoBoneToTorsoVenules.MapPath(TorsoBone1ToTorso2);
    vTorsoVenulesToVenaCava.MapPath(Torso2ToVenaCava2);

    /////////////////////
    // Bleeds and IV's //
    SELiquidCompartmentLink& vVenaCavaIV = m_Compartments->CreateLiquidLink(vGround, vVenaCava, pulse::ExpandedVascularLink::VenaCavaIV);
    vVenaCavaIV.MapPath(IVToVenaCava1);

    SELiquidCompartmentGraph& gCardiovascular = m_Compartments->GetCardiovascularGraph();
    gCardiovascular.AddCompartment(vRightHeart);
    gCardiovascular.AddCompartment(vRightPulmonaryArteries);
    gCardiovascular.AddCompartment(vLeftPulmonaryArteries);
    gCardiovascular.AddCompartment(vRightPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vLeftPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightPulmonaryVeins);
    gCardiovascular.AddCompartment(vLeftPulmonaryVeins);
    gCardiovascular.AddCompartment(vLeftHeart);
    gCardiovascular.AddCompartment(vAorta);
    gCardiovascular.AddCompartment(vArterialBuffer);
    gCardiovascular.AddCompartment(vExtracranial);
    gCardiovascular.AddCompartment(vIntracranial);
    gCardiovascular.AddCompartment(vGut1);
    gCardiovascular.AddCompartment(vLargeIntestine);
    gCardiovascular.AddCompartment(vLiver);
    gCardiovascular.AddCompartment(vLeftArmArterioles);
    gCardiovascular.AddCompartment(vLeftArmBone);
    gCardiovascular.AddCompartment(vLeftArmFat);
    gCardiovascular.AddCompartment(vLeftArmMicrovasculature);
    gCardiovascular.AddCompartment(vLeftArmMuscle);
    gCardiovascular.AddCompartment(vLeftArmSkin);
    gCardiovascular.AddCompartment(vLeftArmVenules);
    gCardiovascular.AddCompartment(vLeftKidney);
    gCardiovascular.AddCompartment(vLeftLegArterioles);
    gCardiovascular.AddCompartment(vLeftLegBone);
    gCardiovascular.AddCompartment(vLeftLegFat);
    gCardiovascular.AddCompartment(vLeftLegMicrovasculature);
    gCardiovascular.AddCompartment(vLeftLegMuscle);
    gCardiovascular.AddCompartment(vLeftLegSkin);
    gCardiovascular.AddCompartment(vLeftLegVenules);
    gCardiovascular.AddCompartment(vMyocardium);
    gCardiovascular.AddCompartment(vRightArmArterioles);
    gCardiovascular.AddCompartment(vRightArmBone);
    gCardiovascular.AddCompartment(vRightArmFat);
    gCardiovascular.AddCompartment(vRightArmMicrovasculature);
    gCardiovascular.AddCompartment(vRightArmMuscle);
    gCardiovascular.AddCompartment(vRightArmSkin);
    gCardiovascular.AddCompartment(vRightArmVenules);
    gCardiovascular.AddCompartment(vRightKidney);
    gCardiovascular.AddCompartment(vRightLegArterioles);
    gCardiovascular.AddCompartment(vRightLegBone);
    gCardiovascular.AddCompartment(vRightLegFat);
    gCardiovascular.AddCompartment(vRightLegMicrovasculature);
    gCardiovascular.AddCompartment(vRightLegMuscle);
    gCardiovascular.AddCompartment(vRightLegSkin);
    gCardiovascular.AddCompartment(vRightLegVenules);
    gCardiovascular.AddCompartment(vSmallIntestine);
    gCardiovascular.AddCompartment(vSplanchnic);
    gCardiovascular.AddCompartment(vSpleen);
    gCardiovascular.AddCompartment(vTorsoArterioles);
    gCardiovascular.AddCompartment(vTorsoBone);
    gCardiovascular.AddCompartment(vTorsoFat);
    gCardiovascular.AddCompartment(vTorsoMicrovasculature);
    gCardiovascular.AddCompartment(vTorsoMuscle);
    gCardiovascular.AddCompartment(vTorsoSkin);
    gCardiovascular.AddCompartment(vTorsoVenules);
    gCardiovascular.AddCompartment(vVenaCava);
    gCardiovascular.AddCompartment(vVenousBuffer);
    gCardiovascular.AddLink(vVenousBufferToVenaCava);
    gCardiovascular.AddLink(vVenaCavaToRightHeart);
    gCardiovascular.AddLink(vRightHeartToLeftPulmonaryArteries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToCapillaries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToVeins);
    gCardiovascular.AddLink(vLeftPulmonaryCapillariesToVeins);
    gCardiovascular.AddLink(vLeftPulmonaryVeinsToLeftHeart);
    gCardiovascular.AddLink(vRightHeartToRightPulmonaryArteries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToVeins);
    gCardiovascular.AddLink(vRightPulmonaryCapillariesToVeins);
    gCardiovascular.AddLink(vRightPulmonaryVeinsToLeftHeart);
    gCardiovascular.AddLink(vLeftHeartToAorta);
    gCardiovascular.AddLink(vAortaToArterialBuffer);
    gCardiovascular.AddLink(vAortaToExtracranial);
    gCardiovascular.AddLink(vExtracranialToVenaCava);
    gCardiovascular.AddLink(vAortaToIntracranial);
    gCardiovascular.AddLink(vIntracranialToVenaCava);
    gCardiovascular.AddLink(vAortaToLargeIntestine);
    gCardiovascular.AddLink(vLargeIntestineToLiver);

    gCardiovascular.AddLink(vAortaToLeftArmArterioles);
    gCardiovascular.AddLink(vLeftArmArteriolesToLeftArmBone);
    gCardiovascular.AddLink(vLeftArmArteriolesToLeftArmFat);
    gCardiovascular.AddLink(vLeftArmArteriolesToLeftArmMicrovasculature);
    gCardiovascular.AddLink(vLeftArmArteriolesToLeftArmMuscle);
    gCardiovascular.AddLink(vLeftArmArteriolesToLeftArmSkin);

    gCardiovascular.AddLink(vAortaToLeftKidney);
    gCardiovascular.AddLink(vLeftKidneyToVenaCava);

    gCardiovascular.AddLink(vAortaToLeftLegArterioles);
    gCardiovascular.AddLink(vLeftLegArteriolesToLeftLegBone);
    gCardiovascular.AddLink(vLeftLegArteriolesToLeftLegFat);
    gCardiovascular.AddLink(vLeftLegArteriolesToLeftLegMicrovasculature);
    gCardiovascular.AddLink(vLeftLegArteriolesToLeftLegMuscle);
    gCardiovascular.AddLink(vLeftLegArteriolesToLeftLegSkin);
    gCardiovascular.AddLink(vAortaToLiver);
    gCardiovascular.AddLink(vLiverToVenaCava);
    gCardiovascular.AddLink(vAortaToMyocardium);
    gCardiovascular.AddLink(vMyocardiumToVenaCava);

    gCardiovascular.AddLink(vAortaToRightArmArterioles);
    gCardiovascular.AddLink(vRightArmArteriolesToRightArmFat);
    gCardiovascular.AddLink(vRightArmArteriolesToRightArmMuscle);
    gCardiovascular.AddLink(vRightArmArteriolesToRightArmSkin);
    gCardiovascular.AddLink(vRightArmArteriolesToRightArmBone);
    gCardiovascular.AddLink(vRightArmArteriolesToRightArmMicrovasculature);

    gCardiovascular.AddLink(vRightArmBoneToRightArmVenules);
    gCardiovascular.AddLink(vRightArmFatToRightArmVenules);
    gCardiovascular.AddLink(vRightArmMicrovasculatureToRightArmVenules);
    gCardiovascular.AddLink(vRightArmMuscleToRightArmVenules);
    gCardiovascular.AddLink(vRightArmSkinToRightArmVenules);
    gCardiovascular.AddLink(vRightArmVenulesToVenaCava);

    gCardiovascular.AddLink(vRightLegBoneToRightLegVenules);
    gCardiovascular.AddLink(vRightLegFatToRightLegVenules);
    gCardiovascular.AddLink(vRightLegMicrovasculatureToRightLegVenules);
    gCardiovascular.AddLink(vRightLegMuscleToRightLegVenules);
    gCardiovascular.AddLink(vRightLegSkinToRightLegVenules);
    gCardiovascular.AddLink(vRightLegVenulesToVenaCava);

    gCardiovascular.AddLink(vLeftArmBoneToLeftArmVenules);
    gCardiovascular.AddLink(vLeftArmFatToLeftArmVenules);
    gCardiovascular.AddLink(vLeftArmMicrovasculatureToLeftArmVenules);
    gCardiovascular.AddLink(vLeftArmMuscleToLeftArmVenules);
    gCardiovascular.AddLink(vLeftArmSkinToLeftArmVenules);
    gCardiovascular.AddLink(vLeftArmVenulesToVenaCava);

    gCardiovascular.AddLink(vLeftLegBoneToLeftLegVenules);
    gCardiovascular.AddLink(vLeftLegFatToLeftLegVenules);
    gCardiovascular.AddLink(vLeftLegMicrovasculatureToLeftLegVenules);
    gCardiovascular.AddLink(vLeftLegMuscleToLeftLegVenules);
    gCardiovascular.AddLink(vLeftLegSkinToLeftLegVenules);
    gCardiovascular.AddLink(vLeftLegVenulesToVenaCava);

    gCardiovascular.AddLink(vTorsoBoneToTorsoVenules);
    gCardiovascular.AddLink(vTorsoFatToTorsoVenules);
    gCardiovascular.AddLink(vTorsoMicrovasculatureToTorsoVenules);
    gCardiovascular.AddLink(vTorsoMuscleToTorsoVenules);
    gCardiovascular.AddLink(vTorsoSkinToTorsoVenules);
    gCardiovascular.AddLink(vTorsoVenulesToVenaCava);

    gCardiovascular.AddLink(vAortaToRightKidney);
    gCardiovascular.AddLink(vRightKidneyToVenaCava);

    gCardiovascular.AddLink(vAortaToRightLegArterioles);
    gCardiovascular.AddLink(vRightLegArteriolesToRightLegFat);
    gCardiovascular.AddLink(vRightLegArteriolesToRightLegMuscle);
    gCardiovascular.AddLink(vRightLegArteriolesToRightLegSkin);
    gCardiovascular.AddLink(vRightLegArteriolesToRightLegBone);
    gCardiovascular.AddLink(vRightLegArteriolesToRightLegMicrovasculature);

    gCardiovascular.AddLink(vAortaToGut);
    gCardiovascular.AddLink(vAortaToSmallIntestine);
    gCardiovascular.AddLink(vSmallIntestineToLiver);
    gCardiovascular.AddLink(vAortaToSplanchnic);
    gCardiovascular.AddLink(vSplanchnicToLiver);
    gCardiovascular.AddLink(vAortaToSpleen);
    gCardiovascular.AddLink(vSpleenToLiver);
    gCardiovascular.AddLink(vAortaToTorsoArterioles);
    gCardiovascular.AddLink(vTorsoArteriolesToTorsoBone);
    gCardiovascular.AddLink(vTorsoArteriolesToTorsoFat);
    gCardiovascular.AddLink(vTorsoArteriolesToTorsoMicrovasculature);
    gCardiovascular.AddLink(vTorsoArteriolesToTorsoMuscle);
    gCardiovascular.AddLink(vTorsoArteriolesToTorsoSkin);
    gCardiovascular.AddLink(vVenaCavaIV);
    gCardiovascular.StateChange();

    SELiquidCompartmentGraph& gCombinedCardiovascular = m_Compartments->GetActiveCardiovascularGraph();
    gCombinedCardiovascular.AddGraph(gCardiovascular);
    gCombinedCardiovascular.StateChange();
  }

  void Controller::SetupExpandedCardiovascularRenal()
  {
    Info("Setting Up Expanded Cardiovascular Renal");
    //////////////////////////
    // Circuit Interdependence
    SEFluidCircuit& cCardiovascular = m_Circuits->GetCardiovascularCircuit();

    bool male = m_InitialPatient->GetSex() == ePatient_Sex::Male ? true : false;

    //assuming there is a left and right kidney node in cardiovascular AND that a baseline volume is set (as a function of patient mass):
    double leftKidneyFluidVolume_mL = 0;
    if (cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney1)->HasVolumeBaseline()) { leftKidneyFluidVolume_mL +=cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney1)->GetVolumeBaseline(VolumeUnit::mL);}
    if (cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney2)->HasVolumeBaseline()) { leftKidneyFluidVolume_mL +=cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney2)->GetVolumeBaseline(VolumeUnit::mL);}
    if (cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney3)->HasVolumeBaseline()) { leftKidneyFluidVolume_mL +=cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney3)->GetVolumeBaseline(VolumeUnit::mL);}

    double singleKidneyLargeVasculatureFluidVolume_mL = leftKidneyFluidVolume_mL / 1.5;// / 2;    //Total large vasculature fluid volume
    double singleKidneySmallVasculatureFluidVolume_mL = leftKidneyFluidVolume_mL / 1.5;// / 2;    //Total small vasculature fluid volume 

    //////////////////////////
    ///// Circuit Parameters//////
    double openSwitch_mmHg_s_Per_mL = m_Config->GetDefaultOpenFlowResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //Resistances with some tuning multipliers
    double urineTuningMultiplier = 0.80; //0.85;
    double arteryTuningMultiplier = 1.25;//0.35;

    double renalArteryResistanceFactor = male ? 0.025 : 0.055;

    double renalArteryResistance_mmHg_s_Per_mL = Convert(renalArteryResistanceFactor * arteryTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double afferentResistance_mmHg_s_Per_mL = Convert(0.0417 * arteryTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double efferentResistance_mmHg_s_Per_mL = Convert(0.0763 * arteryTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double glomerularResistance_mmHg_s_Per_mL = Convert(0.0019 * arteryTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double peritubularResistance_mmHg_s_Per_mL = Convert(0.0167 * arteryTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double renalVeinResistance_mmHg_s_Per_mL = Convert(0.0066 * arteryTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double glomerularFilterResistance_mmHg_s_Per_mL = Convert(0.1600 * urineTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double tubulesResistance_mmHg_s_Per_mL = Convert(0.1920 * urineTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double reabsoprtionResistance_mmHg_s_Per_mL = Convert(0.1613 * urineTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //This one is tuned
    double ureterTuningMultiplier = 0.65;
    double ureterResistance_mmHg_s_Per_mL = Convert(30.0 * ureterTuningMultiplier, PressureTimePerVolumeUnit::mmHg_min_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double urethraResistance_mmHg_s_Per_mL = openSwitch_mmHg_s_Per_mL;
    //Compliances
    //0.5 * CapacitanceKidney is the per-kidney value from 3 element Windkessel
    //double totalCompliance = (0.5 * (0.91 * 1.7560) * 0.02);
    double totalCompliance = 0;
    if (cCardiovascular.HasPath(pulse::ExpandedCardiovascularPath::LeftKidney1ToGround))  { totalCompliance +=cCardiovascular.GetPath(pulse::ExpandedCardiovascularPath::LeftKidney1ToGround)->GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg);}
    if (cCardiovascular.HasPath(pulse::ExpandedCardiovascularPath::LeftKidney2ToGround))  { totalCompliance +=cCardiovascular.GetPath(pulse::ExpandedCardiovascularPath::LeftKidney2ToGround)->GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg);}
    if (cCardiovascular.HasPath(pulse::ExpandedCardiovascularPath::LeftKidney3ToGround))  { totalCompliance +=cCardiovascular.GetPath(pulse::ExpandedCardiovascularPath::LeftKidney3ToGround)->GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg);}
    ///\todo The bladder is currently not being modeled as a compliance
    //double bladderCompliance_mL_Per_mmHg = Convert(38.3, VolumePerPressureUnit::mL_Per_cmH2O, VolumePerPressureUnit::mL_Per_mmHg);

    //Large Vasculature (divide total large vasculature fluid volume three ways): 
    double tubulesVolume_mL = singleKidneyLargeVasculatureFluidVolume_mL / 3.0;
    double renalArteryVolume_mL = singleKidneyLargeVasculatureFluidVolume_mL / 3.0;
    double renalVeinVolume_mL = singleKidneyLargeVasculatureFluidVolume_mL / 3.0;

    //Small vasculature (divide total small vasculature fluid volume five ways):
    double peritubularVolume_mL = singleKidneySmallVasculatureFluidVolume_mL / 5.0;
    double efferentVolume_mL = singleKidneySmallVasculatureFluidVolume_mL / 5.0;
    double afferentVolume_mL = singleKidneySmallVasculatureFluidVolume_mL / 5.0;
    double bowmansVolume_mL = singleKidneySmallVasculatureFluidVolume_mL / 5.0;
    double glomerularVolume_mL = singleKidneySmallVasculatureFluidVolume_mL / 5.0;

    //Using width = 1.8 mm and length = 11 inches => 710.6 mm^3
    double ureterVolume_mL = 0.71;

    //Tuned constants
    double bladderVolume_mL = 1.0;
    //Unstressed Pressures - set to zero to use unstressed properly
    // double renalArteryPressure_mmHg = 0.0;
    // double renalVeinPressure_mmHg = 0.0;
    // double glomerularPressure_mmHg = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney1)->GetPressure(PressureUnit::mmHg);

    double renalArteryPressure_mmHg = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney1)->GetPressure(PressureUnit::mmHg);
    double glomerularPressure_mmHg = 60;
    double renalVeinPressure_mmHg = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney3)->GetPressure(PressureUnit::mmHg);
    //double bladderPressure_mmHg = bladderVolume_mL / bladderCompliance_mL_Per_mmHg;
    //Pressure Sources
    double glomerularOsmoticPressure_mmHg = -32.0;
    double bowmansOsmoticPressure_mmHg = 0.0;
    double tubulesOsmoticPressure_mmHg = -15.0;
    double peritubularOsmoticPressure_mmHg = -32.0;

    SEFluidCircuit& cRenal = m_Circuits->GetRenalCircuit();

    ////////////
    // Ground //
    SEFluidCircuitNode& Ground = cRenal.CreateNode(pulse::RenalNode::Ground);
    Ground.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    Ground.SetAsReferenceNode();

    //////////////////
    // Create Nodes //
    //////////////////

    /////////////////
    // Right Blood //
    /////////////////
    //////////////////////////
    // RightRenalArtery //
    SEFluidCircuitNode& RightRenalArtery = cRenal.CreateNode(pulse::RenalNode::RightRenalArtery);
    RightRenalArtery.GetVolumeBaseline().SetValue(renalArteryVolume_mL, VolumeUnit::mL);
    RightRenalArtery.GetPressure().SetValue(renalArteryPressure_mmHg, PressureUnit::mmHg);
    //RightRenalArtery.GetNextPressure().SetValue(renalArteryPressure_mmHg, PressureUnit::mmHg);
    ////////////////////////////
    // RightAfferentArteriole //
    SEFluidCircuitNode& RightAfferentArteriole = cRenal.CreateNode(pulse::RenalNode::RightAfferentArteriole);
    RightAfferentArteriole.GetVolumeBaseline().SetValue(afferentVolume_mL, VolumeUnit::mL);
    ////////////////////////////////
    // RightGlomerularCapillaries //
    SEFluidCircuitNode& RightGlomerularCapillaries = cRenal.CreateNode(pulse::RenalNode::RightGlomerularCapillaries);
    RightGlomerularCapillaries.GetVolumeBaseline().SetValue(glomerularVolume_mL, VolumeUnit::mL);
    RightGlomerularCapillaries.GetPressure().SetValue(glomerularPressure_mmHg, PressureUnit::mmHg);
    ////////////////////////////
    // RightEfferentArteriole //
    SEFluidCircuitNode& RightEfferentArteriole = cRenal.CreateNode(pulse::RenalNode::RightEfferentArteriole);
    RightEfferentArteriole.GetVolumeBaseline().SetValue(efferentVolume_mL, VolumeUnit::mL);
    /////////////////////////////////
    // RightPeritubularCapillaries // 
    SEFluidCircuitNode& RightPeritubularCapillaries = cRenal.CreateNode(pulse::RenalNode::RightPeritubularCapillaries);
    RightPeritubularCapillaries.GetVolumeBaseline().SetValue(peritubularVolume_mL, VolumeUnit::mL);
    ////////////////////
    // RightRenalVein // 
    SEFluidCircuitNode& RightRenalVein = cRenal.CreateNode(pulse::RenalNode::RightRenalVein);
    RightRenalVein.GetVolumeBaseline().SetValue(renalVeinVolume_mL, VolumeUnit::mL);
    RightRenalVein.GetPressure().SetValue(renalVeinPressure_mmHg, PressureUnit::mmHg);
    //////////////////////////
    // RightBowmansCapsules //
    SEFluidCircuitNode& RightBowmansCapsules = cRenal.CreateNode(pulse::RenalNode::RightBowmansCapsules);
    RightBowmansCapsules.GetVolumeBaseline().SetValue(bowmansVolume_mL, VolumeUnit::mL);
    /////////////////////////////
    // RightNetBowmansCapsules //
    SEFluidCircuitNode& RightNetBowmansCapsules = cRenal.CreateNode(pulse::RenalNode::RightNetBowmansCapsules);
    ///////////////////////////////////
    // RightNetGlomerularCapillaries // 
    SEFluidCircuitNode& RightNetGlomerularCapillaries = cRenal.CreateNode(pulse::RenalNode::RightNetGlomerularCapillaries);
    ////////////////////////////////////
    // RightNetPeritubularCapillaries //
    SEFluidCircuitNode& RightNetPeritubularCapillaries = cRenal.CreateNode(pulse::RenalNode::RightNetPeritubularCapillaries);
    /////////////////
    // Right Urine //
    /////////////////
    //////////////////
    // RightTubules //
    SEFluidCircuitNode& RightTubules = cRenal.CreateNode(pulse::RenalNode::RightTubules);
    RightTubules.GetVolumeBaseline().SetValue(tubulesVolume_mL, VolumeUnit::mL);
    /////////////////////
    // RightNetTubules //
    SEFluidCircuitNode& RightNetTubules = cRenal.CreateNode(pulse::RenalNode::RightNetTubules);
    /////////////////
    // RightUreter //
    SEFluidCircuitNode& RightUreter = cRenal.CreateNode(pulse::RenalNode::RightUreter);
    RightUreter.GetVolumeBaseline().SetValue(ureterVolume_mL, VolumeUnit::mL);

    /////////////////
    // Left Blood //
    /////////////////
    /////////////////////
    // LeftRenalArtery //
    SEFluidCircuitNode& LeftRenalArtery = cRenal.CreateNode(pulse::RenalNode::LeftRenalArtery);
    LeftRenalArtery.GetVolumeBaseline().SetValue(renalArteryVolume_mL, VolumeUnit::mL);
    LeftRenalArtery.GetPressure().SetValue(renalArteryPressure_mmHg, PressureUnit::mmHg);
    //LeftRenalArtery.GetNextPressure().SetValue(renalArteryPressure_mmHg, PressureUnit::mmHg);
    ////////////////////////////
    // LeftAfferentArteriole //
    SEFluidCircuitNode& LeftAfferentArteriole = cRenal.CreateNode(pulse::RenalNode::LeftAfferentArteriole);
    LeftAfferentArteriole.GetVolumeBaseline().SetValue(afferentVolume_mL, VolumeUnit::mL);
    ////////////////////////////////
    // LeftGlomerularCapillaries //
    SEFluidCircuitNode& LeftGlomerularCapillaries = cRenal.CreateNode(pulse::RenalNode::LeftGlomerularCapillaries);
    LeftGlomerularCapillaries.GetVolumeBaseline().SetValue(glomerularVolume_mL, VolumeUnit::mL);
    LeftGlomerularCapillaries.GetPressure().SetValue(glomerularPressure_mmHg, PressureUnit::mmHg);
    ////////////////////////////
    // LeftEfferentArteriole //
    SEFluidCircuitNode& LeftEfferentArteriole = cRenal.CreateNode(pulse::RenalNode::LeftEfferentArteriole);
    LeftEfferentArteriole.GetVolumeBaseline().SetValue(efferentVolume_mL, VolumeUnit::mL);
    /////////////////////////////////
    // LeftPeritubularCapillaries // 
    SEFluidCircuitNode& LeftPeritubularCapillaries = cRenal.CreateNode(pulse::RenalNode::LeftPeritubularCapillaries);
    LeftPeritubularCapillaries.GetVolumeBaseline().SetValue(peritubularVolume_mL, VolumeUnit::mL);
    ////////////////////
    // LeftRenalVein // 
    SEFluidCircuitNode& LeftRenalVein = cRenal.CreateNode(pulse::RenalNode::LeftRenalVein);
    LeftRenalVein.GetVolumeBaseline().SetValue(renalVeinVolume_mL, VolumeUnit::mL);
    LeftRenalVein.GetPressure().SetValue(renalVeinPressure_mmHg, PressureUnit::mmHg);

    //////////////////////////
    // LeftBowmansCapsules //
    SEFluidCircuitNode& LeftBowmansCapsules = cRenal.CreateNode(pulse::RenalNode::LeftBowmansCapsules);
    LeftBowmansCapsules.GetVolumeBaseline().SetValue(bowmansVolume_mL, VolumeUnit::mL);
    /////////////////////////////
    // LeftNetBowmansCapsules //
    SEFluidCircuitNode& LeftNetBowmansCapsules = cRenal.CreateNode(pulse::RenalNode::LeftNetBowmansCapsules);
    ///////////////////////////////////
    // LeftNetGlomerularCapillaries // 
    SEFluidCircuitNode& LeftNetGlomerularCapillaries = cRenal.CreateNode(pulse::RenalNode::LeftNetGlomerularCapillaries);
    ////////////////////////////////////
    // LeftNetPeritubularCapillaries //
    SEFluidCircuitNode& LeftNetPeritubularCapillaries = cRenal.CreateNode(pulse::RenalNode::LeftNetPeritubularCapillaries);
    /////////////////
    // Left Urine //
    /////////////////  
    //////////////////
    // LeftTubules //
    SEFluidCircuitNode& LeftTubules = cRenal.CreateNode(pulse::RenalNode::LeftTubules);
    LeftTubules.GetVolumeBaseline().SetValue(tubulesVolume_mL, VolumeUnit::mL);
    /////////////////////
    // LeftNetTubules //
    SEFluidCircuitNode& LeftNetTubules = cRenal.CreateNode(pulse::RenalNode::LeftNetTubules);
    /////////////////
    // LeftUreter //
    SEFluidCircuitNode& LeftUreter = cRenal.CreateNode(pulse::RenalNode::LeftUreter);
    LeftUreter.GetVolumeBaseline().SetValue(ureterVolume_mL, VolumeUnit::mL);

    /////////////
    // Bladder //
    SEFluidCircuitNode& Bladder = cRenal.CreateNode(pulse::RenalNode::Bladder);
    Bladder.GetVolumeBaseline().SetValue(bladderVolume_mL, VolumeUnit::mL);
    //Bladder.GetPressure().SetValue(0.0, PressureUnit::mmHg);

    //////////////////
    // Create Paths //
    //////////////////

    /////////////////
    // Right Blood //
    /////////////////
    //////////////////////
    // RightRenalArteryToAfferentArteriole //
    SEFluidCircuitPath& RightRenalArteryToAfferentArteriole = cRenal.CreatePath(RightRenalArtery, RightAfferentArteriole, pulse::RenalPath::RightRenalArteryToAfferentArteriole);
    RightRenalArteryToAfferentArteriole.GetResistanceBaseline().SetValue(renalArteryResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    ////////////////////////////////
    // RightRenalArteryCompliance //
    SEFluidCircuitPath& RightRenalArteryCompliance = cRenal.CreatePath(RightRenalArtery, Ground, pulse::RenalPath::RightRenalArteryCompliance);
    RightRenalArteryCompliance.GetComplianceBaseline().SetValue(renalArteryVolume_mL/renalArteryPressure_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    ///////////////////////////////////////////////////
    // RightAfferentArterioleToGlomerularCapillaries //
    SEFluidCircuitPath& RightAfferentArterioleToGlomerularCapillaries = cRenal.CreatePath(RightAfferentArteriole, RightGlomerularCapillaries, pulse::RenalPath::RightAfferentArterioleToGlomerularCapillaries);
    RightAfferentArterioleToGlomerularCapillaries.GetResistanceBaseline().SetValue(afferentResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    ///////////////////////////////////////////////////
    // RightGlomerularCapillariesToEfferentArteriole //
    SEFluidCircuitPath& RightGlomerularCapillariesToEfferentArteriole = cRenal.CreatePath(RightGlomerularCapillaries, RightEfferentArteriole, pulse::RenalPath::RightGlomerularCapillariesToEfferentArteriole);
    RightGlomerularCapillariesToEfferentArteriole.GetResistanceBaseline().SetValue(glomerularResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //////////////////////////////////////////
    // RightGlomerularCapillariesCompliance //
    SEFluidCircuitPath& RightGlomerularCapillariesCompliance = cRenal.CreatePath(RightGlomerularCapillaries, Ground, pulse::RenalPath::RightGlomerularCapillariesCompliance);
    RightGlomerularCapillariesCompliance.GetComplianceBaseline().SetValue(glomerularVolume_mL/glomerularPressure_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    ////////////////////////////////////////////////////
    // RightEfferentArterioleToPeritubularCapillaries //
    SEFluidCircuitPath& RightEfferentArterioleToPeritubularCapillaries = cRenal.CreatePath(RightEfferentArteriole, RightPeritubularCapillaries, pulse::RenalPath::RightEfferentArterioleToPeritubularCapillaries);
    RightEfferentArterioleToPeritubularCapillaries.GetResistanceBaseline().SetValue(efferentResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    ////////////////////////////////////////////
    // RightPeritubularCapillariesToRenalVein //
    SEFluidCircuitPath& RightPeritubularCapillariesToRenalVein = cRenal.CreatePath(RightPeritubularCapillaries, RightRenalVein, pulse::RenalPath::RightPeritubularCapillariesToRenalVein);
    RightPeritubularCapillariesToRenalVein.GetResistanceBaseline().SetValue(peritubularResistance_mmHg_s_Per_mL + renalVeinResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //////////////////////////////
    // RightRenalVeinCompliance //
    SEFluidCircuitPath& RightRenalVeinCompliance = cRenal.CreatePath(RightRenalVein, Ground, pulse::RenalPath::RightRenalVeinCompliance);
    RightRenalVeinCompliance.GetComplianceBaseline().SetValue(renalVeinVolume_mL/renalVeinPressure_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    //////////////////////////////////////////////////////////
    // RightGlomerularCapillariesToNetGlomerularCapillaries //
    SEFluidCircuitPath& RightGlomerularCapillariesToNetGlomerularCapillaries = cRenal.CreatePath(RightGlomerularCapillaries, RightNetGlomerularCapillaries, pulse::RenalPath::RightGlomerularCapillariesToNetGlomerularCapillaries);
    RightGlomerularCapillariesToNetGlomerularCapillaries.GetPressureSourceBaseline().SetValue(glomerularOsmoticPressure_mmHg, PressureUnit::mmHg);
    ///////////////////////////////////////////////////////
    // RightNetGlomerularCapillariesToNetBowmansCapsules //
    SEFluidCircuitPath& RightNetGlomerularCapillariesToNetBowmansCapsules = cRenal.CreatePath(RightNetGlomerularCapillaries, RightNetBowmansCapsules, pulse::RenalPath::RightNetGlomerularCapillariesToNetBowmansCapsules);
    RightNetGlomerularCapillariesToNetBowmansCapsules.GetResistanceBaseline().SetValue(glomerularFilterResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //////////////////////////////////////////////
    // RightBowmansCapsulesToNetBowmansCapsules //
    SEFluidCircuitPath& RightBowmansCapsulesToNetBowmansCapsules = cRenal.CreatePath(RightBowmansCapsules, RightNetBowmansCapsules, pulse::RenalPath::RightBowmansCapsulesToNetBowmansCapsules);
    RightBowmansCapsulesToNetBowmansCapsules.GetPressureSourceBaseline().SetValue(bowmansOsmoticPressure_mmHg, PressureUnit::mmHg);
    /////////////////
    // Right Urine //
    ///////////////// 
    ///////////////////////////////////
    // RightBowmansCapsulesToTubules //
    SEFluidCircuitPath& RightBowmansCapsulesToTubules = cRenal.CreatePath(RightBowmansCapsules, RightTubules, pulse::RenalPath::RightBowmansCapsulesToTubules);
    RightBowmansCapsulesToTubules.GetResistanceBaseline().SetValue(tubulesResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //////////////////////////
    // RightTubulesToUreter //
    SEFluidCircuitPath& RightTubulesToUreter = cRenal.CreatePath(RightTubules, RightUreter, pulse::RenalPath::RightTubulesToUreter);
    RightTubulesToUreter.GetResistanceBaseline().SetValue(ureterResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    /////////////////////////////
    // RighTubulesToNetTubules //
    SEFluidCircuitPath& RightTubulesToNetTubules = cRenal.CreatePath(RightTubules, RightNetTubules, pulse::RenalPath::RightTubulesToNetTubules);
    RightTubulesToNetTubules.GetPressureSourceBaseline().SetValue(tubulesOsmoticPressure_mmHg, PressureUnit::mmHg);
    ////////////////////////////////////////////////
    // RightNetTubulesToNetPeritubularCapillaries //
    SEFluidCircuitPath& RightNetTubulesToNetPeritubularCapillaries = cRenal.CreatePath(RightNetTubules, RightNetPeritubularCapillaries, pulse::RenalPath::RightNetTubulesToNetPeritubularCapillaries);
    RightNetTubulesToNetPeritubularCapillaries.GetResistanceBaseline().SetValue(reabsoprtionResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    ////////////////////////////////////////////////////////////
    // RightPeritubularCapillariesToNetPeritubularCapillaries //
    SEFluidCircuitPath& RightPeritubularCapillariesToNetPeritubularCapillaries = cRenal.CreatePath(RightPeritubularCapillaries, RightNetPeritubularCapillaries, pulse::RenalPath::RightPeritubularCapillariesToNetPeritubularCapillaries);
    RightPeritubularCapillariesToNetPeritubularCapillaries.GetPressureSourceBaseline().SetValue(peritubularOsmoticPressure_mmHg, PressureUnit::mmHg);
    //////////////////////////
    // RightUreterToBladder //
    SEFluidCircuitPath& RightUreterToBladder = cRenal.CreatePath(RightUreter, Bladder, pulse::RenalPath::RightUreterToBladder);
    RightUreterToBladder.SetNextValve(eGate::Closed);

    /////////////////
    // Left Blood //
    /////////////////
    //////////////////////
    // LeftRenalArteryToAfferentArteriole //
    SEFluidCircuitPath& LeftRenalArteryToAfferentArteriole = cRenal.CreatePath(LeftRenalArtery, LeftAfferentArteriole, pulse::RenalPath::LeftRenalArteryToAfferentArteriole);
    LeftRenalArteryToAfferentArteriole.GetResistanceBaseline().SetValue(renalArteryResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    ////////////////////////////////
    // LeftRenalArteryCompliance //
    SEFluidCircuitPath& LeftRenalArteryCompliance = cRenal.CreatePath(LeftRenalArtery, Ground, pulse::RenalPath::LeftRenalArteryCompliance);
    LeftRenalArteryCompliance.GetComplianceBaseline().SetValue(renalArteryVolume_mL/renalArteryPressure_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    ///////////////////////////////////////////////////
    // LeftAfferentArterioleToGlomerularCapillaries //
    SEFluidCircuitPath& LeftAfferentArterioleToGlomerularCapillaries = cRenal.CreatePath(LeftAfferentArteriole, LeftGlomerularCapillaries, pulse::RenalPath::LeftAfferentArterioleToGlomerularCapillaries);
    LeftAfferentArterioleToGlomerularCapillaries.GetResistanceBaseline().SetValue(afferentResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    ///////////////////////////////////////////////////
    // LeftGlomerularCapillariesToEfferentArteriole //
    SEFluidCircuitPath& LeftGlomerularCapillariesToEfferentArteriole = cRenal.CreatePath(LeftGlomerularCapillaries, LeftEfferentArteriole, pulse::RenalPath::LeftGlomerularCapillariesToEfferentArteriole);
    LeftGlomerularCapillariesToEfferentArteriole.GetResistanceBaseline().SetValue(glomerularResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //////////////////////////////////////////
    // LeftGlomerularCapillariesCompliance //
    SEFluidCircuitPath& LeftGlomerularCapillariesCompliance = cRenal.CreatePath(LeftGlomerularCapillaries, Ground, pulse::RenalPath::LeftGlomerularCapillariesCompliance);
    LeftGlomerularCapillariesCompliance.GetComplianceBaseline().SetValue(glomerularVolume_mL/glomerularPressure_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    ////////////////////////////////////////////////////
    // LeftEfferentArterioleToPeritubularCapillaries //
    SEFluidCircuitPath& LeftEfferentArterioleToPeritubularCapillaries = cRenal.CreatePath(LeftEfferentArteriole, LeftPeritubularCapillaries, pulse::RenalPath::LeftEfferentArterioleToPeritubularCapillaries);
    LeftEfferentArterioleToPeritubularCapillaries.GetResistanceBaseline().SetValue(efferentResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    ////////////////////////////////////////////
    // LeftPeritubularCapillariesToRenalVein //
    SEFluidCircuitPath& LeftPeritubularCapillariesToRenalVein = cRenal.CreatePath(LeftPeritubularCapillaries, LeftRenalVein, pulse::RenalPath::LeftPeritubularCapillariesToRenalVein);
    LeftPeritubularCapillariesToRenalVein.GetResistanceBaseline().SetValue(peritubularResistance_mmHg_s_Per_mL + renalVeinResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //////////////////////////////
    // LeftRenalVeinCompliance //
    SEFluidCircuitPath& LeftRenalVeinCompliance = cRenal.CreatePath(LeftRenalVein, Ground, pulse::RenalPath::LeftRenalVeinCompliance);
    LeftRenalVeinCompliance.GetComplianceBaseline().SetValue(renalVeinVolume_mL/renalVeinPressure_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    //////////////////////////////////////////////////////////
    // LeftGlomerularCapillariesToNetGlomerularCapillaries //
    SEFluidCircuitPath& LeftGlomerularCapillariesToNetGlomerularCapillaries = cRenal.CreatePath(LeftGlomerularCapillaries, LeftNetGlomerularCapillaries, pulse::RenalPath::LeftGlomerularCapillariesToNetGlomerularCapillaries);
    LeftGlomerularCapillariesToNetGlomerularCapillaries.GetPressureSourceBaseline().SetValue(glomerularOsmoticPressure_mmHg, PressureUnit::mmHg);
    ///////////////////////////////////////////////////////
    // LeftNetGlomerularCapillariesToNetBowmansCapsules //
    SEFluidCircuitPath& LeftNetGlomerularCapillariesToNetBowmansCapsules = cRenal.CreatePath(LeftNetGlomerularCapillaries, LeftNetBowmansCapsules, pulse::RenalPath::LeftNetGlomerularCapillariesToNetBowmansCapsules);
    LeftNetGlomerularCapillariesToNetBowmansCapsules.GetResistanceBaseline().SetValue(glomerularFilterResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //////////////////////////////////////////////
    // LeftBowmansCapsulesToNetBowmansCapsules //
    SEFluidCircuitPath& LeftBowmansCapsulesToNetBowmansCapsules = cRenal.CreatePath(LeftBowmansCapsules, LeftNetBowmansCapsules, pulse::RenalPath::LeftBowmansCapsulesToNetBowmansCapsules);
    LeftBowmansCapsulesToNetBowmansCapsules.GetPressureSourceBaseline().SetValue(bowmansOsmoticPressure_mmHg, PressureUnit::mmHg);
    /////////////////
    // Left Urine //
    /////////////////  
    ///////////////////////////////////
    // LeftBowmansCapsulesToTubules //
    SEFluidCircuitPath& LeftBowmansCapsulesToTubules = cRenal.CreatePath(LeftBowmansCapsules, LeftTubules, pulse::RenalPath::LeftBowmansCapsulesToTubules);
    LeftBowmansCapsulesToTubules.GetResistanceBaseline().SetValue(tubulesResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //////////////////////////
    // LeftTubulesToUreter //
    SEFluidCircuitPath& LeftTubulesToUreter = cRenal.CreatePath(LeftTubules, LeftUreter, pulse::RenalPath::LeftTubulesToUreter);
    LeftTubulesToUreter.GetResistanceBaseline().SetValue(ureterResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    /////////////////////////////
    // LeftubulesToNetTubules //
    SEFluidCircuitPath& LeftTubulesToNetTubules = cRenal.CreatePath(LeftTubules, LeftNetTubules, pulse::RenalPath::LeftTubulesToNetTubules);
    LeftTubulesToNetTubules.GetPressureSourceBaseline().SetValue(tubulesOsmoticPressure_mmHg, PressureUnit::mmHg);
    ////////////////////////////////////////////////
    // LeftNetTubulesToNetPeritubularCapillaries //
    SEFluidCircuitPath& LeftNetTubulesToNetPeritubularCapillaries = cRenal.CreatePath(LeftNetTubules, LeftNetPeritubularCapillaries, pulse::RenalPath::LeftNetTubulesToNetPeritubularCapillaries);
    LeftNetTubulesToNetPeritubularCapillaries.GetResistanceBaseline().SetValue(reabsoprtionResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    ////////////////////////////////////////////////////////////
    // LeftPeritubularCapillariesToNetPeritubularCapillaries //
    SEFluidCircuitPath& LeftPeritubularCapillariesToNetPeritubularCapillaries = cRenal.CreatePath(LeftPeritubularCapillaries, LeftNetPeritubularCapillaries, pulse::RenalPath::LeftPeritubularCapillariesToNetPeritubularCapillaries);
    LeftPeritubularCapillariesToNetPeritubularCapillaries.GetPressureSourceBaseline().SetValue(peritubularOsmoticPressure_mmHg, PressureUnit::mmHg);
    //////////////////////////
    // LeftUreterToBladder //
    SEFluidCircuitPath& LeftUreterToBladder = cRenal.CreatePath(LeftUreter, Bladder, pulse::RenalPath::LeftUreterToBladder);
    LeftUreterToBladder.SetNextValve(eGate::Closed);

    ///////////////////////
    // BladderCompliance //
    SEFluidCircuitPath& BladderToGroundPressure = cRenal.CreatePath(Bladder, Ground, pulse::RenalPath::BladderToGroundPressure);
    /// \todo Use a compliance here - make sure you remove the current handling of bladder volume in the renal system as a pressure source
    //BladderCompliance.GetComplianceBaseline().SetValue(bladderCompliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    BladderToGroundPressure.GetPressureSourceBaseline().SetValue(-4.0, PressureUnit::mmHg); //Negative because source-target is for compliance
    //////////////
    // BladderGround //
    SEFluidCircuitPath& BladderToGroundUrinate = cRenal.CreatePath(Bladder, Ground, pulse::RenalPath::BladderToGroundUrinate);
    BladderToGroundUrinate.GetResistanceBaseline().SetValue(urethraResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    cRenal.SetNextAndCurrentFromBaselines();
    cRenal.StateChange();

    // Delete the three-element Windkessel kidney model 
    SEFluidCircuit& cCombinedCardiovascular = m_Circuits->GetActiveCardiovascularCircuit();
    m_Circuits->DeleteFluidNode(pulse::ExpandedCardiovascularNode::LeftKidney1);
    m_Circuits->DeleteFluidNode(pulse::ExpandedCardiovascularNode::LeftKidney2);
    m_Circuits->DeleteFluidNode(pulse::ExpandedCardiovascularNode::LeftKidney3);
    m_Circuits->DeleteFluidNode(pulse::ExpandedCardiovascularNode::RightKidney1);
    m_Circuits->DeleteFluidNode(pulse::ExpandedCardiovascularNode::RightKidney3);
    m_Circuits->DeleteFluidNode(pulse::ExpandedCardiovascularNode::RightKidney2);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::Aorta5ToLeftKidney1);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::Aorta5ToRightKidney1);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::LeftKidney1ToLeftKidney2);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::RightKidney1ToRightKidney2);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::LeftKidney2ToGround);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::RightKidney2ToGround);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::LeftKidney2ToLeftKidney3);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::RightKidney2ToRightKidney3);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::LeftKidney3ToVenaCava2);
    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::RightKidney3ToVenaCava2);
    // Remove modifiers on the proximal resistor (aorta side) of paths we removed
    m_Config->GetModifiers().erase(pulse::CardiovascularPath::LeftKidney1ToLeftKidney2);
    m_Config->GetModifiers().erase(pulse::CardiovascularPath::RightKidney1ToRightKidney2);

    cCombinedCardiovascular.AddCircuit(cRenal);
    // Grab the nodes that we will be connecting between the 2 circuits
    SEFluidCircuitNode* Aorta5 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Aorta5);
    SEFluidCircuitNode* VenaCava2 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::VenaCava2);
    // Add the new connection paths
    SEFluidCircuitPath& Aorta5ToRightKidney1 = cCombinedCardiovascular.CreatePath(*Aorta5, RightRenalArtery, pulse::ExpandedCardiovascularPath::Aorta5ToRightKidney1);
    SEFluidCircuitPath& RightKidney3ToVenaCava2 = cCombinedCardiovascular.CreatePath(RightRenalVein, *VenaCava2, pulse::ExpandedCardiovascularPath::RightKidney3ToVenaCava2);
    SEFluidCircuitPath& Aorta5ToLeftKidney1 = cCombinedCardiovascular.CreatePath(*Aorta5, LeftRenalArtery, pulse::ExpandedCardiovascularPath::Aorta5ToLeftKidney1);
    SEFluidCircuitPath& LeftKidney3ToVenaCava2 = cCombinedCardiovascular.CreatePath(LeftRenalVein, *VenaCava2, pulse::ExpandedCardiovascularPath::LeftKidney3ToVenaCava2);
    
    // Update the circuit
    cCombinedCardiovascular.SetNextAndCurrentFromBaselines();
    cCombinedCardiovascular.StateChange();

    ////////////////////////
    // Renal Compartments //
    ////////////////////////

    ///////////
    // Blood //
    ///////////

    ///////////////////////
    // RightRenalArtery //
    SELiquidCompartment& vRightRenalArtery = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightRenalArtery);
    vRightRenalArtery.MapNode(RightRenalArtery);
    //////////////////////////////
    // RightAfferentArteriole //
    SELiquidCompartment& vRightAfferentArteriole = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightAfferentArteriole);
    vRightAfferentArteriole.MapNode(RightAfferentArteriole);
    ////////////////////////////////
    // RightGlomerularCapillaries //
    SELiquidCompartment& vRightGlomerularCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightGlomerularCapillaries);
    vRightGlomerularCapillaries.MapNode(RightGlomerularCapillaries);
    vRightGlomerularCapillaries.MapNode(RightNetGlomerularCapillaries);
    ////////////////////////////
    // RightEfferentArteriole //
    SELiquidCompartment& vRightEfferentArteriole = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightEfferentArteriole);
    vRightEfferentArteriole.MapNode(RightEfferentArteriole);
    /////////////////////////////////
    // RightPeritubularCapillaries //
    SELiquidCompartment& vRightPeritubularCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightPeritubularCapillaries);
    vRightPeritubularCapillaries.MapNode(RightPeritubularCapillaries);
    vRightPeritubularCapillaries.MapNode(RightNetPeritubularCapillaries);
    ///////////////////
    // RightRenalVein //
    SELiquidCompartment& vRightRenalVein = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightRenalVein);
    vRightRenalVein.MapNode(RightRenalVein);
    //////////////////////////
    // RightBowmansCapsules //
    SELiquidCompartment& vRightBowmansCapsules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightBowmansCapsules);
    vRightBowmansCapsules.MapNode(RightBowmansCapsules);
    vRightBowmansCapsules.MapNode(RightNetBowmansCapsules);
    //////////////////
    // RightTubules //
    SELiquidCompartment& vRightTubules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightTubules);
    vRightTubules.MapNode(RightTubules);
    vRightTubules.MapNode(RightNetTubules);

    /////////////////////
    // LeftRenalArtery //
    SELiquidCompartment& vLeftRenalArtery = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftRenalArtery);
    vLeftRenalArtery.MapNode(LeftRenalArtery);
    //////////////////////////////
    // LeftAfferentArteriole //
    SELiquidCompartment& vLeftAfferentArteriole = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftAfferentArteriole);
    vLeftAfferentArteriole.MapNode(LeftAfferentArteriole);
    ////////////////////////////////
    // LeftGlomerularCapillaries //
    SELiquidCompartment& vLeftGlomerularCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftGlomerularCapillaries);
    vLeftGlomerularCapillaries.MapNode(LeftGlomerularCapillaries);
    vLeftGlomerularCapillaries.MapNode(LeftNetGlomerularCapillaries);
    ////////////////////////////
    // LeftEfferentArteriole //
    SELiquidCompartment& vLeftEfferentArteriole = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftEfferentArteriole);
    vLeftEfferentArteriole.MapNode(LeftEfferentArteriole);
    ////////////////////////////////
    // LeftPeritubularCapillaries //
    SELiquidCompartment& vLeftPeritubularCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftPeritubularCapillaries);
    vLeftPeritubularCapillaries.MapNode(LeftPeritubularCapillaries);
    vLeftPeritubularCapillaries.MapNode(LeftNetPeritubularCapillaries);
    ///////////////////
    // LeftRenalVein //
    SELiquidCompartment& vLeftRenalVein = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftRenalVein);
    vLeftRenalVein.MapNode(LeftRenalVein);
    /////////////////////////
    // LeftBowmansCapsules //
    SELiquidCompartment& vLeftBowmansCapsules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftBowmansCapsules);
    vLeftBowmansCapsules.MapNode(LeftBowmansCapsules);
    vLeftBowmansCapsules.MapNode(LeftNetBowmansCapsules);
    /////////////////
    // LeftTubules //
    SELiquidCompartment& vLeftTubules = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftTubules);
    vLeftTubules.MapNode(LeftTubules);
    vLeftTubules.MapNode(LeftNetTubules);

    // Let's build out the hierarchy
    // Grab these, as cardiovascular already made them  
    SELiquidCompartment* vLeftKidney = m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftKidney);
    SELiquidCompartment& vLeftNephron = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::LeftNephron);
    vLeftKidney->GetNodeMapping().Clear();// Remove the nodes the cardiovascular was using to model the kidney
    vLeftKidney->AddChild(vLeftRenalArtery);
    vLeftKidney->AddChild(vLeftNephron);
    vLeftNephron.AddChild(vLeftAfferentArteriole);
    vLeftNephron.AddChild(vLeftGlomerularCapillaries);
    vLeftNephron.AddChild(vLeftEfferentArteriole);
    vLeftNephron.AddChild(vLeftPeritubularCapillaries);
    vLeftNephron.AddChild(vLeftBowmansCapsules);
    vLeftNephron.AddChild(vLeftTubules);
    vLeftKidney->AddChild(vLeftRenalVein);
    SELiquidCompartment* vRightKidney = m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightKidney);
    SELiquidCompartment& vRightNephron = m_Compartments->CreateLiquidCompartment(pulse::ExpandedVascularCompartment::RightNephron);
    vRightKidney->GetNodeMapping().Clear();// Remove the nodes the cardiovascular was using to model the kidney
    vRightKidney->AddChild(vRightRenalArtery);
    vRightKidney->AddChild(vRightNephron);
    vRightNephron.AddChild(vRightAfferentArteriole);
    vRightNephron.AddChild(vRightGlomerularCapillaries);
    vRightNephron.AddChild(vRightEfferentArteriole);
    vRightNephron.AddChild(vRightPeritubularCapillaries);
    vRightNephron.AddChild(vRightBowmansCapsules);
    vRightNephron.AddChild(vRightTubules);
    vRightKidney->AddChild(vRightRenalVein);

    ///////////
    // Urine //
    ///////////

    /////////////////
    // RightUreter //
    SELiquidCompartment& uRightUreter = m_Compartments->CreateLiquidCompartment(pulse::UrineCompartment::RightUreter);
    uRightUreter.MapNode(RightUreter);
    ////////////////
    // LeftUreter //
    SELiquidCompartment& uLeftUreter = m_Compartments->CreateLiquidCompartment(pulse::UrineCompartment::LeftUreter);
    uLeftUreter.MapNode(LeftUreter);
    ////////////////
    // Ureters //
    SELiquidCompartment& uUreters = m_Compartments->CreateLiquidCompartment(pulse::UrineCompartment::Ureters);
    uUreters.AddChild(uRightUreter);
    uUreters.AddChild(uLeftUreter);
    /////////////
    // Bladder //
    SELiquidCompartment& uBladder = m_Compartments->CreateLiquidCompartment(pulse::UrineCompartment::Bladder);
    uBladder.MapNode(Bladder);

    /////////////////////////////
    // Renal Compartment Links //
    /////////////////////////////

    // Graph Dependencies
    SELiquidCompartment& vArterialBuffer = *m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::ArterialBuffer);
    SELiquidCompartment& vVenousBuffer = *m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::VenousBuffer);
    SELiquidCompartment& vGround = *m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::Ground);

    ///////////
    // Blood //
    ///////////

    ////////////////////////////
    // AortaToRightRenalArtery //
    m_Compartments->DeleteLiquidLink(pulse::ExpandedVascularLink::AortaToRightKidney);// Replace this link
    SELiquidCompartmentLink& vAortaToRightRenalArtery = m_Compartments->CreateLiquidLink(vArterialBuffer, vRightRenalArtery, pulse::ExpandedVascularLink::AortaToRightKidney);
    vAortaToRightRenalArtery.MapPath(Aorta5ToRightKidney1);
    ////////////////////////////////////////
    // RightRenalArteryToAfferentArteriole //
    SELiquidCompartmentLink& vRightRenalArteryToAfferentArteriole = m_Compartments->CreateLiquidLink(vRightRenalArtery, vRightAfferentArteriole, pulse::ExpandedVascularLink::RightRenalArteryToAfferentArteriole);
    vRightRenalArteryToAfferentArteriole.MapPath(RightRenalArteryToAfferentArteriole);
    //////////////////////////////////////////////////
    // RightAfferentArterioleToGlomerularCapillaries //
    SELiquidCompartmentLink& vRightAfferentArterioleToGlomerularCapillaries = m_Compartments->CreateLiquidLink(vRightAfferentArteriole, vRightGlomerularCapillaries, pulse::ExpandedVascularLink::RightAfferentArterioleToGlomerularCapillaries);
    vRightAfferentArterioleToGlomerularCapillaries.MapPath(RightAfferentArterioleToGlomerularCapillaries);
    //////////////////////////////////////////////////
    // RightGlomerularCapillariesToEfferentArteriole //
    SELiquidCompartmentLink& vRightGlomerularCapillariesToEfferentArteriole = m_Compartments->CreateLiquidLink(vRightGlomerularCapillaries, vRightEfferentArteriole, pulse::ExpandedVascularLink::RightGlomerularCapillariesToEfferentArteriole);
    vRightGlomerularCapillariesToEfferentArteriole.MapPath(RightGlomerularCapillariesToEfferentArteriole);
    /////////////////////////////////////////////////
    // RightGlomerularCapillariesToBowmansCapsules //
    SELiquidCompartmentLink& vRightGlomerularCapillariesToBowmansCapsules = m_Compartments->CreateLiquidLink(vRightGlomerularCapillaries, vRightBowmansCapsules, pulse::ExpandedVascularLink::RightGlomerularCapillariesToBowmansCapsules);
    vRightGlomerularCapillariesToBowmansCapsules.MapPath(RightNetGlomerularCapillariesToNetBowmansCapsules);
    ///////////////////////////////////
    // RightBowmansCapsulesToTubules //
    SELiquidCompartmentLink& vRightBowmansCapsulesToTubules = m_Compartments->CreateLiquidLink(vRightBowmansCapsules, vRightTubules, pulse::ExpandedVascularLink::RightBowmansCapsulesToTubules);
    vRightBowmansCapsulesToTubules.MapPath(RightBowmansCapsulesToTubules);
    //////////////////////////////////////////
    // RightTubulesToPeritubularCapillaries //
    SELiquidCompartmentLink& vRightTubulesToPeritubularCapillaries = m_Compartments->CreateLiquidLink(vRightTubules, vRightPeritubularCapillaries, pulse::ExpandedVascularLink::RightTubulesToPeritubularCapillaries);
    vRightTubulesToPeritubularCapillaries.MapPath(RightNetTubulesToNetPeritubularCapillaries);
    ///////////////////////////////////////////////////
    // RightEfferentArterioleToPeritubularCapillaries //
    SELiquidCompartmentLink& vRightEfferentArterioleToPeritubularCapillaries = m_Compartments->CreateLiquidLink(vRightEfferentArteriole, vRightPeritubularCapillaries, pulse::ExpandedVascularLink::RightEfferentArterioleToPeritubularCapillaries);
    vRightEfferentArterioleToPeritubularCapillaries.MapPath(RightEfferentArterioleToPeritubularCapillaries);
    ////////////////////////////////////////////
    // RightPeritubularCapillariesToRenalVein //
    SELiquidCompartmentLink& vRightPeritubularCapillariesToRenalVein = m_Compartments->CreateLiquidLink(vRightPeritubularCapillaries, vRightRenalVein, pulse::ExpandedVascularLink::RightPeritubularCapillariesToRenalVein);
    vRightPeritubularCapillariesToRenalVein.MapPath(RightPeritubularCapillariesToRenalVein);
    /////////////////////////////
    // RightRenalVeinToVenaCava //
    m_Compartments->DeleteLiquidLink(pulse::ExpandedVascularLink::RightKidneyToVenaCava);// Replace this vink
    SELiquidCompartmentLink& vRightRenalVeinToVenaCava = m_Compartments->CreateLiquidLink(vRightRenalVein, vVenousBuffer, pulse::ExpandedVascularLink::RightKidneyToVenaCava);
    vRightRenalVeinToVenaCava.MapPath(RightKidney3ToVenaCava2);

    ////////////////////////////
    // AortaToLeftRenalArtery //
    m_Compartments->DeleteLiquidLink(pulse::ExpandedVascularLink::AortaToLeftKidney);// Replace this uink
    SELiquidCompartmentLink& vAortaToLeftRenalArtery = m_Compartments->CreateLiquidLink(vArterialBuffer, vLeftRenalArtery, pulse::ExpandedVascularLink::AortaToLeftKidney);
    vAortaToLeftRenalArtery.MapPath(Aorta5ToLeftKidney1);  
    ////////////////////////////////////////
    // LeftRenalArteryToAfferentArteriole //
    SELiquidCompartmentLink& vLeftRenalArteryToAfferentArteriole = m_Compartments->CreateLiquidLink(vLeftRenalArtery, vLeftAfferentArteriole, pulse::ExpandedVascularLink::LeftRenalArteryToAfferentArteriole);
    vLeftRenalArteryToAfferentArteriole.MapPath(LeftRenalArteryToAfferentArteriole);
    //////////////////////////////////////////////////
    // LeftAfferentArterioleToGlomerularCapillaries //
    SELiquidCompartmentLink& vLeftAfferentArterioleToGlomerularCapillaries = m_Compartments->CreateLiquidLink(vLeftAfferentArteriole, vLeftGlomerularCapillaries, pulse::ExpandedVascularLink::LeftAfferentArterioleToGlomerularCapillaries);
    vLeftAfferentArterioleToGlomerularCapillaries.MapPath(LeftAfferentArterioleToGlomerularCapillaries);
    //////////////////////////////////////////////////
    // LeftGlomerularCapillariesToEfferentArteriole //
    SELiquidCompartmentLink& vLeftGlomerularCapillariesToEfferentArteriole = m_Compartments->CreateLiquidLink(vLeftGlomerularCapillaries, vLeftEfferentArteriole, pulse::ExpandedVascularLink::LeftGlomerularCapillariesToEfferentArteriole);
    vLeftGlomerularCapillariesToEfferentArteriole.MapPath(LeftGlomerularCapillariesToEfferentArteriole);
    ////////////////////////////////////////////////
    // LeftGlomerularCapillariesToBowmansCapsules //
    SELiquidCompartmentLink& vLeftGlomerularCapillariesToBowmansCapsules = m_Compartments->CreateLiquidLink(vLeftGlomerularCapillaries, vLeftBowmansCapsules, pulse::ExpandedVascularLink::LeftGlomerularCapillariesToBowmansCapsules);
    vLeftGlomerularCapillariesToBowmansCapsules.MapPath(LeftNetGlomerularCapillariesToNetBowmansCapsules);
    //////////////////////////////////
    // LeftBowmansCapsulesToTubules //
    SELiquidCompartmentLink& vLeftBowmansCapsulesToTubules = m_Compartments->CreateLiquidLink(vLeftBowmansCapsules, vLeftTubules, pulse::ExpandedVascularLink::LeftBowmansCapsulesToTubules);
    vLeftBowmansCapsulesToTubules.MapPath(LeftBowmansCapsulesToTubules);
    /////////////////////////////////////////
    // LeftTubulesToPeritubularCapillaries //
    SELiquidCompartmentLink& vLeftTubulesToPeritubularCapillaries = m_Compartments->CreateLiquidLink(vLeftTubules, vLeftPeritubularCapillaries, pulse::ExpandedVascularLink::LeftTubulesToPeritubularCapillaries);
    vLeftTubulesToPeritubularCapillaries.MapPath(LeftNetTubulesToNetPeritubularCapillaries);
    ///////////////////////////////////////////////////
    // LeftEfferentArterioleToPeritubularCapillaries //
    SELiquidCompartmentLink& vLeftEfferentArterioleToPeritubularCapillaries = m_Compartments->CreateLiquidLink(vLeftEfferentArteriole, vLeftPeritubularCapillaries, pulse::ExpandedVascularLink::LeftEfferentArterioleToPeritubularCapillaries);
    vLeftEfferentArterioleToPeritubularCapillaries.MapPath(LeftEfferentArterioleToPeritubularCapillaries);
    ///////////////////////////////////////////
    // LeftPeritubularCapillariesToRenalVein //
    SELiquidCompartmentLink& vLeftPeritubularCapillariesToRenalVein = m_Compartments->CreateLiquidLink(vLeftPeritubularCapillaries, vLeftRenalVein, pulse::ExpandedVascularLink::LeftPeritubularCapillariesToRenalVein);
    vLeftPeritubularCapillariesToRenalVein.MapPath(LeftPeritubularCapillariesToRenalVein);
    /////////////////////////////
    // LeftRenalVeinToVenaCava //
    m_Compartments->DeleteLiquidLink(pulse::ExpandedVascularLink::LeftKidneyToVenaCava);// Replace this link
    SELiquidCompartmentLink& vLeftRenalVeinToVenaCava = m_Compartments->CreateLiquidLink(vLeftRenalVein, vVenousBuffer, pulse::ExpandedVascularLink::LeftKidneyToVenaCava);
    vLeftRenalVeinToVenaCava.MapPath(LeftKidney3ToVenaCava2);

    ///////////
    // Urine //
    ///////////

    //////////////////////////
    // RightTubulesToUreter //
    SELiquidCompartmentLink& uRightTubulesToUreter = m_Compartments->CreateLiquidLink(vRightTubules, uRightUreter, pulse::UrineLink::RightTubulesToUreter);
    uRightTubulesToUreter.MapPath(RightTubulesToUreter);
    //////////////////////////
    // RightUreterToBladder //
    SELiquidCompartmentLink& uRightUreterToBladder = m_Compartments->CreateLiquidLink(uRightUreter, uBladder, pulse::UrineLink::RightUreterToBladder);
    uRightUreterToBladder.MapPath(RightUreterToBladder);


    /////////////////////////
    // LeftTubulesToUreter //
    SELiquidCompartmentLink& uLeftTubulesToUreter = m_Compartments->CreateLiquidLink(vLeftTubules, uLeftUreter, pulse::UrineLink::LeftTubulesToUreter);
    uLeftTubulesToUreter.MapPath(LeftTubulesToUreter);
    /////////////////////////
    // LeftUreterToBladder //
    SELiquidCompartmentLink& uLeftUreterToBladder = m_Compartments->CreateLiquidLink(uLeftUreter, uBladder, pulse::UrineLink::LeftUreterToBladder);
    uLeftUreterToBladder.MapPath(LeftUreterToBladder);

    /////////////////////
    // BladderToGround //
    SELiquidCompartmentLink& uBladderToGround = m_Compartments->CreateLiquidLink(uBladder, vGround, pulse::UrineLink::BladderToGround);
    uBladderToGround.MapPath(BladderToGroundUrinate);
    SELiquidCompartmentLink& uBladderToGroundSource = m_Compartments->CreateLiquidLink(uBladder, vGround, pulse::UrineLink::BladderToGroundSource);
    uBladderToGroundSource.MapPath(BladderToGroundPressure);

    SELiquidCompartmentGraph& gRenal = m_Compartments->GetRenalGraph();
    gRenal.AddCompartment(vArterialBuffer);
    gRenal.AddCompartment(vVenousBuffer);
    // Left Blood
    gRenal.AddCompartment(vLeftRenalArtery);
    gRenal.AddCompartment(vLeftAfferentArteriole);
    gRenal.AddCompartment(vLeftGlomerularCapillaries);
    gRenal.AddCompartment(vLeftEfferentArteriole);
    gRenal.AddCompartment(vLeftPeritubularCapillaries);
    gRenal.AddCompartment(vLeftBowmansCapsules);
    gRenal.AddCompartment(vLeftTubules);
    gRenal.AddCompartment(vLeftRenalVein);
    gRenal.AddLink(vAortaToLeftRenalArtery);
    gRenal.AddLink(vLeftRenalArteryToAfferentArteriole);
    gRenal.AddLink(vLeftAfferentArterioleToGlomerularCapillaries);
    gRenal.AddLink(vLeftGlomerularCapillariesToEfferentArteriole);
    //gRenal.AddLink(vLeftGlomerularCapillariesToBowmansCapsules); //Active transport only
    gRenal.AddLink(vLeftBowmansCapsulesToTubules);
    //gRenal.AddLink(vLeftTubulesToPeritubularCapillaries); //Active transport only
    gRenal.AddLink(vLeftEfferentArterioleToPeritubularCapillaries);
    gRenal.AddLink(vLeftPeritubularCapillariesToRenalVein);
    gRenal.AddLink(vLeftRenalVeinToVenaCava);
    // Right Blood
    gRenal.AddCompartment(vRightRenalArtery);
    gRenal.AddCompartment(vRightAfferentArteriole);
    gRenal.AddCompartment(vRightGlomerularCapillaries);
    gRenal.AddCompartment(vRightEfferentArteriole);
    gRenal.AddCompartment(vRightPeritubularCapillaries);
    gRenal.AddCompartment(vRightBowmansCapsules);
    gRenal.AddCompartment(vRightTubules);
    gRenal.AddCompartment(vRightRenalVein);
    gRenal.AddLink(vAortaToRightRenalArtery);
    gRenal.AddLink(vRightRenalArteryToAfferentArteriole);
    gRenal.AddLink(vRightAfferentArterioleToGlomerularCapillaries);
    gRenal.AddLink(vRightGlomerularCapillariesToEfferentArteriole);
    //gRenal.AddLink(vRightGlomerularCapillariesToBowmansCapsules); //Active transport only
    gRenal.AddLink(vRightBowmansCapsulesToTubules);
    //gRenal.AddLink(vRightTubulesToPeritubularCapillaries); //Active transport only
    gRenal.AddLink(vRightEfferentArterioleToPeritubularCapillaries);
    gRenal.AddLink(vRightPeritubularCapillariesToRenalVein);
    gRenal.AddLink(vRightRenalVeinToVenaCava);
    // Left Urine
    gRenal.AddCompartment(uLeftUreter);
    gRenal.AddLink(uLeftTubulesToUreter);
    gRenal.AddLink(uLeftUreterToBladder);
    // Right Urine
    gRenal.AddCompartment(uRightUreter);
    gRenal.AddLink(uRightTubulesToUreter);
    gRenal.AddLink(uRightUreterToBladder);
    // Shared
    gRenal.AddCompartment(uBladder);
    gRenal.AddCompartment(vGround);
    gRenal.AddLink(uBladderToGround);
    gRenal.AddLink(uBladderToGroundSource);
    gRenal.StateChange();

    // We have discretized these compartments, so remove them
    SELiquidCompartmentGraph& gCombinedCardiovascular = m_Compartments->GetActiveCardiovascularGraph();
    gCombinedCardiovascular.RemoveCompartment(*vLeftKidney);
    gCombinedCardiovascular.RemoveCompartment(*vRightKidney);
    gCombinedCardiovascular.AddGraph(gRenal);
    gCombinedCardiovascular.StateChange();
  }

  void Controller::SetupExpandedCardiovascularTissue()
  {
    Info("Setting Up Expanded Cardiovascular Tissue");
    SEFluidCircuit& cCardiovascular = m_Circuits->GetCardiovascularCircuit();
    SEFluidCircuit& cCombinedCardiovascular = m_Circuits->GetActiveCardiovascularCircuit();

    SEFluidCircuitNode* Ground = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Ground);
    ///////////
    // Lymph //
    SEFluidCircuitNode& Lymph = cCombinedCardiovascular.CreateNode(pulse::TissueNode::Lymph);
    Lymph.GetPressure().SetValue(0, PressureUnit::mmHg);
    Lymph.GetVolumeBaseline().SetValue(0, VolumeUnit::mL);

    SEFluidCircuitNode* VenaCava1 = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::VenaCava1);
    SEFluidCircuitPath& LymphToVenaCava = cCombinedCardiovascular.CreatePath(Lymph, *VenaCava1, pulse::ExpandedCardiovascularTissuePath::LymphToVenaCava);
    // No resistance - if it goes to lymph then it goes to vena cava
    SELiquidCompartment* cVenaCava = m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::VenaCava);

    SELiquidCompartment& cLymph = m_Compartments->CreateLiquidCompartment(pulse::LymphCompartment::Lymph);
    cLymph.MapNode(Lymph);

    SELiquidCompartmentLink& lLymphToVenaCava = m_Compartments->CreateLiquidLink(cLymph, *cVenaCava, pulse::LymphLink::LymphToVenaCava);
    lLymphToVenaCava.MapPath(LymphToVenaCava);

    // The assumption for the vascular-tissue convection drag is that the resistance is inversely proportional to the tissue mass.
    // This is the proportionality constant
    double resistanceConstant = 20000.0; // Large resistance to prevent fluid from freely moving into the vascular space

    /// \todo Put Initial Circuit/Compartment data values into the configuration file.

    //Density (kg/L)
    double AdiposeTissueDensity = 1.03;
    double BoneTissueDensity = 1.3;
    double BrainTissueDensity = 1.0;
    double GutTissueDensity = 1.0;
    double RKidneyTissueDensity = 1.0;
    double LKidneyTissueDensity = 1.0;
    double LiverTissueDensity = 1.0;
    double RLungTissueDensity = 1.0;
    double LLungTissueDensity = 1.0;
    double MuscleTissueDensity = 1.0;
    double MyocardiumTissueDensity = 1.0;
    double SkinTissueDensity = 1.0;
    double SpleenTissueDensity = 1.0;

    // ExtracellcularWaterFraction        IntracellularWaterFraction    NeutralLipid                   NeutralPhospolipid             AlbuminRatio              AlphaAcidGlycoprotein       PlasmaLipoprotein        AcidicPhospohlipidConcentration
    double  AdiposeEWFraction = 0.135, AdiposeIWFraction = 0.017, AdiposeNLFraction = 0.79, AdiposeNPFraction = 0.002, AdiposeARatio = 0.049, AdiposeAAGRatio = 0.049, AdiposeLRatio = 0.068, AdiposeAPL = 0.4;
    double  BoneEWFraction = 0.1, BoneIWFraction = 0.346, BoneNLFraction = 0.074, BoneNPFraction = 0.0011, BoneARatio = 0.1, BoneAAGRatio = 0.1, BoneLRatio = 0.05, BoneAPL = 0.67;
    double  BrainEWFraction = 0.162, BrainIWFraction = 0.62, BrainNLFraction = 0.051, BrainNPFraction = 0.0565, BrainARatio = 0.048, BrainAAGRatio = 0.048, BrainLRatio = 0.041, BrainAPL = 0.4;
    double  GutEWFraction = 0.282, GutIWFraction = 0.475, GutNLFraction = 0.0487, GutNPFraction = 0.0163, GutARatio = 0.158, GutAAGRatio = 0.158, GutLRatio = 0.0141, GutAPL = 2.41;
    double  RKidneyEWFraction = 0.273, RKidneyIWFraction = 0.483, RKidneyNLFraction = 0.0207, RKidneyNPFraction = 0.0162, RKidneyARatio = 0.13, RKidneyAAGRatio = 0.13, RKidneyLRatio = 0.137, RKidneyAPL = 5.03;
    double  LKidneyEWFraction = 0.273, LKidneyIWFraction = 0.483, LKidneyNLFraction = 0.0207, LKidneyNPFraction = 0.0162, LKidneyARatio = 0.13, LKidneyAAGRatio = 0.13, LKidneyLRatio = 0.137, LKidneyAPL = 5.03;
    double  LiverEWFraction = 0.161, LiverIWFraction = 0.573, LiverNLFraction = 0.0348, LiverNPFraction = 0.0252, LiverARatio = 0.086, LiverAAGRatio = 0.086, LiverLRatio = 0.161, LiverAPL = 4.56;
    double  RLungEWFraction = 0.336, RLungIWFraction = 0.446, RLungNLFraction = 0.003, RLungNPFraction = 0.009, RLungARatio = 0.212, RLungAAGRatio = 0.212, RLungLRatio = 0.168, RLungAPL = 3.91;
    double  LLungEWFraction = 0.336, LLungIWFraction = 0.446, LLungNLFraction = 0.003, LLungNPFraction = 0.009, LLungARatio = 0.212, LLungAAGRatio = 0.212, LLungLRatio = 0.168, LLungAPL = 3.91;
    double  MuscleEWFraction = 0.118, MuscleIWFraction = 0.63, MuscleNLFraction = 0.0238, MuscleNPFraction = 0.0072, MuscleARatio = 0.064, MuscleAAGRatio = 0.064, MuscleLRatio = 0.059, MuscleAPL = 1.53;
    double  MyocardiumEWFraction = 0.32, MyocardiumIWFraction = 0.456, MyocardiumNLFraction = 0.0115, MyocardiumNPFraction = 0.0166, MyocardiumARatio = 0.157, MyocardiumAAGRatio = 0.157, MyocardiumLRatio = 0.16, MyocardiumAPL = 2.25;
    double  SkinEWFraction = 0.382, SkinIWFraction = 0.291, SkinNLFraction = 0.0284, SkinNPFraction = 0.0111, SkinARatio = 0.277, SkinAAGRatio = 0.277, SkinLRatio = 0.096, SkinAPL = 1.32;
    double  SpleenEWFraction = 0.207, SpleenIWFraction = 0.579, SpleenNLFraction = 0.0201, SpleenNPFraction = 0.0198, SpleenARatio = 0.277, SpleenAAGRatio = 0.277, SpleenLRatio = 0.096, SpleenAPL = 3.18;

    //Typical ICRP Male
    //Total Mass (kg)              
    double AdiposeTissueMass = 14.5;
    double BoneTissueMass = 10.5;
    double BrainTissueMass = 1.45;
    double GutTissueMass = 1.02;
    double RKidneyTissueMass = 0.155;
    double LKidneyTissueMass = 0.155;
    double LiverTissueMass = 1.8;
    double RLungTissueMass = 0.25;
    double LLungTissueMass = 0.25;
    double MuscleTissueMass = 29.0;
    double MyocardiumTissueMass = 0.33;
    double SkinTissueMass = 3.3;
    double SpleenTissueMass = 0.15;

    //Typical ICRP Female - From ICRP
    //Total Mass (kg)
    if (m_InitialPatient->GetSex() == ePatient_Sex::Female)
    {
      AdiposeTissueMass = 19.0;
      BoneTissueMass = 7.8;
      BrainTissueMass = 1.3;
      GutTissueMass = 0.96;
      RKidneyTissueMass = 0.1375;
      LKidneyTissueMass = 0.1375;
      LiverTissueMass = 1.4;
      RLungTissueMass = 0.21;
      LLungTissueMass = 0.21;
      MuscleTissueMass = 17.5;
      MyocardiumTissueMass = 0.25;
      SkinTissueMass = 2.3;
      SpleenTissueMass = 0.13;
    }

    //Scale things based on patient parameters -------------------------------

    //Modify adipose (i.e. fat) directly using the body fat fraction
    AdiposeTissueMass = m_InitialPatient->GetBodyFatFraction().GetValue() * m_InitialPatient->GetWeight().GetValue(MassUnit::kg);

    //Modify skin based on total surface area
    //Male
    double standardPatientWeight_lb = 170.0;
    double standardPatientHeight_in = 71.0;
    if (m_InitialPatient->GetSex() == ePatient_Sex::Female)
    {
      //Female
      standardPatientWeight_lb = 130.0;
      standardPatientHeight_in = 64.0;
    }
    double typicalSkinSurfaceArea_m2 = 0.20247 * pow(Convert(standardPatientWeight_lb, MassUnit::lb, MassUnit::kg), 0.425) * pow(Convert(standardPatientHeight_in, LengthUnit::in, LengthUnit::m), 0.725);
    double patientSkinArea_m2 = m_InitialPatient->GetSkinSurfaceArea(AreaUnit::m2);
    SkinTissueMass = SkinTissueMass * patientSkinArea_m2 / typicalSkinSurfaceArea_m2;

    //Modify most based on lean body mass
    //Hume, R (Jul 1966). "Prediction of lean body mass from height and weight." Journal of clinical pathology. 19 (4): 389�91. doi:10.1136/jcp.19.4.389. PMC 473290. PMID 5929341.
    //double typicalLeanBodyMass_kg = 0.32810 * Convert(standardPatientWeight_lb, MassUnit::lb, MassUnit::kg) + 0.33929 * Convert(standardPatientHeight_in, LengthUnit::in, LengthUnit::cm) - 29.5336; //Male
    //if (m_InitialPatient->GetSex() == ePatient_Sex::Female)
    //{
     // typicalLeanBodyMass_kg = 0.29569 * Convert(standardPatientWeight_lb, MassUnit::lb, MassUnit::kg) + 0.41813 * Convert(standardPatientHeight_in, LengthUnit::in, LengthUnit::cm) - 43.2933; //Female
    //}

    //Male
    double standardFatFraction = 0.21;
    if (m_InitialPatient->GetSex() == ePatient_Sex::Female)
    {
      //Female
      standardFatFraction = 0.28;
    }
    double standardLeanBodyMass_kg = Convert(standardPatientWeight_lb, MassUnit::lb, MassUnit::kg) * (1.0 - standardFatFraction);
    double patientLeanBodyMass_kg = m_InitialPatient->GetLeanBodyMass(MassUnit::kg);
    double leanBodyMassFractionOfTypical = patientLeanBodyMass_kg / standardLeanBodyMass_kg;

    BoneTissueMass *= leanBodyMassFractionOfTypical;
    GutTissueMass *= leanBodyMassFractionOfTypical;
    RKidneyTissueMass *= leanBodyMassFractionOfTypical;
    LKidneyTissueMass *= leanBodyMassFractionOfTypical;
    LiverTissueMass *= leanBodyMassFractionOfTypical;
    RLungTissueMass *= leanBodyMassFractionOfTypical;
    LLungTissueMass *= leanBodyMassFractionOfTypical;
    MuscleTissueMass *= leanBodyMassFractionOfTypical;
    MyocardiumTissueMass *= leanBodyMassFractionOfTypical;
    SpleenTissueMass *= leanBodyMassFractionOfTypical;

    //Note: Brain doesn't change

    //Total Volume(L)
    double AdiposeTissueVolume = AdiposeTissueMass / AdiposeTissueDensity;
    double BoneTissueVolume = BoneTissueMass / BoneTissueDensity;
    double BrainTissueVolume = BrainTissueMass / BrainTissueDensity;
    double GutTissueVolume = GutTissueMass / GutTissueDensity;
    double RKidneyTissueVolume = RKidneyTissueMass / RKidneyTissueDensity;
    double LKidneyTissueVolume = LKidneyTissueMass / LKidneyTissueDensity;
    double LiverTissueVolume = LiverTissueMass / LiverTissueDensity;
    double RLungTissueVolume = RLungTissueMass / RLungTissueDensity;
    double LLungTissueVolume = LLungTissueMass / LLungTissueDensity;
    double MuscleTissueVolume = MuscleTissueMass / MuscleTissueDensity;
    double MyocardiumTissueVolume = MyocardiumTissueMass / MyocardiumTissueDensity;
    double SkinTissueVolume = SkinTissueMass / SkinTissueDensity;
    double SpleenTissueVolume = SpleenTissueMass / SpleenTissueDensity;

    //Create the circuit -------------------------------

    // Note on colloid osmotic pressure
    // Vascular oncotic pressure sources are currently set to zero, and a constant oncotic pressure is incorporated
    // into the hydrostatic pressure stored in the tissue compliance elements.
    // In a future release the oncotic pressure sources will be set dynamically based on the Landis-Pappenheimer equation

    /////////
    // Fat //
    SEFluidCircuitNode* TorsoFat1 = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::TorsoFat1);
    SEFluidCircuitNode* LeftArmFat1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftArmFat1);
    SEFluidCircuitNode* LeftLegFat1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftLegFat1);
    SEFluidCircuitNode* RightArmFat1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightArmFat1);
    SEFluidCircuitNode* RightLegFat1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightLegFat1);
    SEFluidCircuitNode& FatT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::FatT1);
    SEFluidCircuitNode& FatT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::FatT2);
    SEFluidCircuitNode& FatT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::FatT3);
    FatT1.GetPressure().Set(TorsoFat1->GetPressure());
    FatT3.GetPressure().Set(Ground->GetPressure());
    FatT1.GetVolumeBaseline().SetValue(AdiposeEWFraction * AdiposeTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& TorsoFat1ToFatT2 = cCombinedCardiovascular.CreatePath(*TorsoFat1, FatT2, pulse::ExpandedCardiovascularTissuePath::TorsoFat1ToFatT2);
    TorsoFat1ToFatT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftArmFat1ToFatT2 = cCombinedCardiovascular.CreatePath(*LeftArmFat1, FatT2, pulse::ExpandedCardiovascularTissuePath::LeftArmFat1ToFatT2);
    LeftArmFat1ToFatT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftLegFat1ToFatT2 = cCombinedCardiovascular.CreatePath(*LeftLegFat1, FatT2, pulse::ExpandedCardiovascularTissuePath::LeftLegFat1ToFatT2);
    LeftLegFat1ToFatT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightArmFat1ToFatT2 = cCombinedCardiovascular.CreatePath(*RightArmFat1, FatT2, pulse::ExpandedCardiovascularTissuePath::RightArmFat1ToFatT2);
    RightArmFat1ToFatT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightLegFat1ToFatT2 = cCombinedCardiovascular.CreatePath(*RightLegFat1, FatT2, pulse::ExpandedCardiovascularTissuePath::RightLegFat1ToFatT2);
    RightLegFat1ToFatT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& FatT2ToFatT1 = cCombinedCardiovascular.CreatePath(FatT2, FatT1, pulse::ExpandedCardiovascularTissuePath::FatT2ToFatT1);
    FatT2ToFatT1.GetResistanceBaseline().SetValue((1 / AdiposeTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& FatT1ToFatT3 = cCombinedCardiovascular.CreatePath(FatT1, FatT3, pulse::ExpandedCardiovascularTissuePath::FatT1ToFatT3);
    FatT1ToFatT3.GetComplianceBaseline().SetValue(FatT1.GetVolumeBaseline(VolumeUnit::mL) / FatT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToFatT3 = cCombinedCardiovascular.CreatePath(*Ground, FatT3, pulse::ExpandedCardiovascularTissuePath::GroundToFatT3);
    GroundToFatT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& FatT1ToLymph = cCombinedCardiovascular.CreatePath(FatT1, Lymph, pulse::ExpandedCardiovascularTissuePath::FatT1ToLymph);
    FatT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& FatTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Fat);
    SELiquidCompartment& FatExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::FatExtracellular);
    SELiquidCompartment& FatIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::FatIntracellular);
    FatTissue.GetMatrixVolume().SetValue((1 - AdiposeEWFraction - AdiposeIWFraction) * AdiposeTissueVolume * 1000.0, VolumeUnit::mL);
    FatExtracellular.MapNode(FatT1);
    FatExtracellular.MapNode(FatT2);
    FatExtracellular.MapNode(FatT3);
    FatExtracellular.GetWaterVolumeFraction().SetValue(AdiposeEWFraction);
    FatIntracellular.GetVolume().SetValue(AdiposeIWFraction * AdiposeTissueVolume * 1000.0, VolumeUnit::mL);
    FatIntracellular.GetWaterVolumeFraction().SetValue(AdiposeIWFraction);
    FatTissue.GetAcidicPhospohlipidConcentration().SetValue(AdiposeAPL, MassPerMassUnit::mg_Per_g);
    FatTissue.GetNeutralLipidsVolumeFraction().SetValue(AdiposeNLFraction);
    FatTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(AdiposeNPFraction);
    FatTissue.GetTissueToPlasmaAlbuminRatio().SetValue(AdiposeARatio);
    FatTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(AdiposeAAGRatio);
    FatTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(AdiposeLRatio);
    FatTissue.GetTotalMass().SetValue(AdiposeTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& TorsoFatVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoFat),
      FatExtracellular, pulse::ExpandedVascularLink::TorsoFatVascularToTissue);
    TorsoFatVascularToTissue.MapPath(TorsoFat1ToFatT2);
    SELiquidCompartmentLink& LeftArmFatVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmFat),
      FatExtracellular, pulse::ExpandedVascularLink::LeftArmFatVascularToTissue);
    LeftArmFatVascularToTissue.MapPath(LeftArmFat1ToFatT2);
    SELiquidCompartmentLink& LeftLegFatVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegFat),
      FatExtracellular, pulse::ExpandedVascularLink::LeftLegFatVascularToTissue);
    LeftLegFatVascularToTissue.MapPath(LeftLegFat1ToFatT2);
    SELiquidCompartmentLink& RightArmFatVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmFat),
      FatExtracellular, pulse::ExpandedVascularLink::RightArmFatVascularToTissue);
    RightArmFatVascularToTissue.MapPath(RightArmFat1ToFatT2);
    SELiquidCompartmentLink& RightLegFatVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegFat),
      FatExtracellular, pulse::ExpandedVascularLink::RightLegFatVascularToTissue);
    RightLegFatVascularToTissue.MapPath(RightLegFat1ToFatT2);

    SELiquidCompartmentLink& FatTissueToLymph = m_Compartments->CreateLiquidLink(FatExtracellular, cLymph, pulse::LymphLink::FatTissueToLymph);
    FatTissueToLymph.MapPath(FatT1ToLymph);

    //////////
    // Bone //
    SEFluidCircuitNode* TorsoBone1 = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::TorsoBone1);
    SEFluidCircuitNode* LeftArmBone1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftArmBone1);
    SEFluidCircuitNode* LeftLegBone1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftLegBone1);
    SEFluidCircuitNode* RightArmBone1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightArmBone1);
    SEFluidCircuitNode* RightLegBone1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightLegBone1);
    SEFluidCircuitNode& BoneT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::BoneT1);
    SEFluidCircuitNode& BoneT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::BoneT2);
    SEFluidCircuitNode& BoneT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::BoneT3);
    BoneT1.GetPressure().Set(TorsoBone1->GetPressure());
    BoneT3.GetPressure().Set(Ground->GetPressure());
    BoneT1.GetVolumeBaseline().SetValue(BoneEWFraction * BoneTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& TorsoBone1ToBoneT2 = cCombinedCardiovascular.CreatePath(*TorsoBone1, BoneT2, pulse::ExpandedCardiovascularTissuePath::TorsoBone1ToBoneT2);
    TorsoBone1ToBoneT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftArmBone1ToBoneT2 = cCombinedCardiovascular.CreatePath(*LeftArmBone1, BoneT2, pulse::ExpandedCardiovascularTissuePath::LeftArmBone1ToBoneT2);
    LeftArmBone1ToBoneT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftLegBone1ToBoneT2 = cCombinedCardiovascular.CreatePath(*LeftLegBone1, BoneT2, pulse::ExpandedCardiovascularTissuePath::LeftLegBone1ToBoneT2);
    LeftLegBone1ToBoneT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightArmBone1ToBoneT2 = cCombinedCardiovascular.CreatePath(*RightArmBone1, BoneT2, pulse::ExpandedCardiovascularTissuePath::RightArmBone1ToBoneT2);
    RightArmBone1ToBoneT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightLegBone1ToBoneT2 = cCombinedCardiovascular.CreatePath(*RightLegBone1, BoneT2, pulse::ExpandedCardiovascularTissuePath::RightLegBone1ToBoneT2);
    RightLegBone1ToBoneT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& BoneT2ToBoneT1 = cCombinedCardiovascular.CreatePath(BoneT2, BoneT1, pulse::ExpandedCardiovascularTissuePath::BoneT2ToBoneT1);
    BoneT2ToBoneT1.GetResistanceBaseline().SetValue((1 / BoneTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& BoneT1ToBoneT3 = cCombinedCardiovascular.CreatePath(BoneT1, BoneT3, pulse::ExpandedCardiovascularTissuePath::BoneT1ToBoneT3);
    BoneT1ToBoneT3.GetComplianceBaseline().SetValue(BoneT1.GetVolumeBaseline(VolumeUnit::mL) / BoneT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToBoneT3 = cCombinedCardiovascular.CreatePath(*Ground, BoneT3, pulse::ExpandedCardiovascularTissuePath::GroundToBoneT3);
    GroundToBoneT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& BoneT1ToLymph = cCombinedCardiovascular.CreatePath(BoneT1, Lymph, pulse::ExpandedCardiovascularTissuePath::BoneT1ToLymph);
    BoneT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& BoneTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Bone);
    SELiquidCompartment& BoneExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::BoneExtracellular);
    SELiquidCompartment& BoneIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::BoneIntracellular);
    BoneTissue.GetMatrixVolume().SetValue((1 - BoneEWFraction - BoneIWFraction) * BoneTissueVolume * 1000.0, VolumeUnit::mL);
    BoneExtracellular.MapNode(BoneT1);
    BoneExtracellular.MapNode(BoneT2);
    BoneExtracellular.MapNode(BoneT3);
    BoneExtracellular.GetWaterVolumeFraction().SetValue(BoneEWFraction);
    BoneIntracellular.GetVolume().SetValue(BoneIWFraction * BoneTissueVolume * 1000.0, VolumeUnit::mL);
    BoneIntracellular.GetWaterVolumeFraction().SetValue(BoneIWFraction);
    BoneTissue.GetAcidicPhospohlipidConcentration().SetValue(BoneAPL, MassPerMassUnit::mg_Per_g);
    BoneTissue.GetNeutralLipidsVolumeFraction().SetValue(BoneNLFraction);
    BoneTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(BoneNPFraction);
    BoneTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(BoneAAGRatio);
    BoneTissue.GetTissueToPlasmaAlbuminRatio().SetValue(BoneARatio);
    BoneTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(BoneLRatio);
    BoneTissue.GetTotalMass().SetValue(BoneTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& TorsoBoneVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoBone),
      BoneExtracellular, pulse::ExpandedVascularLink::TorsoBoneVascularToTissue);
    TorsoBoneVascularToTissue.MapPath(TorsoBone1ToBoneT2);
    SELiquidCompartmentLink& LeftArmBoneVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmBone),
      BoneExtracellular, pulse::ExpandedVascularLink::LeftArmBoneVascularToTissue);
    LeftArmBoneVascularToTissue.MapPath(LeftArmBone1ToBoneT2);
    SELiquidCompartmentLink& LeftLegBoneVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegBone),
      BoneExtracellular, pulse::ExpandedVascularLink::LeftLegBoneVascularToTissue);
    LeftLegBoneVascularToTissue.MapPath(LeftLegBone1ToBoneT2);
    SELiquidCompartmentLink& RightArmBoneVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmBone),
      BoneExtracellular, pulse::ExpandedVascularLink::RightArmBoneVascularToTissue);
    RightArmBoneVascularToTissue.MapPath(RightArmBone1ToBoneT2);
    SELiquidCompartmentLink& RightLegBoneVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegBone),
      BoneExtracellular, pulse::ExpandedVascularLink::RightLegBoneVascularToTissue);
    RightLegBoneVascularToTissue.MapPath(RightLegBone1ToBoneT2);

    SELiquidCompartmentLink& BoneTissueToLymph = m_Compartments->CreateLiquidLink(BoneExtracellular, cLymph, pulse::LymphLink::BoneTissueToLymph);
    BoneTissueToLymph.MapPath(BoneT1ToLymph);

    ///////////
    // Brain //
    SEFluidCircuitNode* Intracranial2 = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Intracranial2);
    SEFluidCircuitNode& BrainT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::BrainT1);
    SEFluidCircuitNode& BrainT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::BrainT2);
    SEFluidCircuitNode& BrainT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::BrainT3);
    BrainT1.GetPressure().Set(Intracranial2->GetPressure());
    BrainT3.GetPressure().Set(Ground->GetPressure());
    BrainT1.GetVolumeBaseline().SetValue(BrainEWFraction * BrainTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& Intracranial1ToBrainT2 = cCombinedCardiovascular.CreatePath(*Intracranial2, BrainT2, pulse::ExpandedCardiovascularTissuePath::Intracranial2ToBrainT2);
    Intracranial1ToBrainT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& BrainT2ToBrainT1 = cCombinedCardiovascular.CreatePath(BrainT2, BrainT1, pulse::ExpandedCardiovascularTissuePath::BrainT2ToBrainT1);
    BrainT2ToBrainT1.GetResistanceBaseline().SetValue((1 / BrainTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& BrainT1ToBrainT3 = cCombinedCardiovascular.CreatePath(BrainT1, BrainT3, pulse::ExpandedCardiovascularTissuePath::BrainT1ToBrainT3);
    BrainT1ToBrainT3.GetComplianceBaseline().SetValue(BrainT1.GetVolumeBaseline(VolumeUnit::mL) / BrainT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToBrainT3 = cCombinedCardiovascular.CreatePath(*Ground, BrainT3, pulse::ExpandedCardiovascularTissuePath::GroundToBrainT3);
    GroundToBrainT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& BrainT1ToLymph = cCombinedCardiovascular.CreatePath(BrainT1, Lymph, pulse::ExpandedCardiovascularTissuePath::BrainT1ToLymph);
    BrainT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& BrainTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Brain);
    SELiquidCompartment& BrainExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::BrainExtracellular);
    SELiquidCompartment& BrainIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::BrainIntracellular);
    BrainTissue.GetMatrixVolume().SetValue((1 - BrainEWFraction - BrainIWFraction) * BrainTissueVolume * 1000.0, VolumeUnit::mL);
    BrainExtracellular.MapNode(BrainT1);
    BrainExtracellular.MapNode(BrainT2);
    BrainExtracellular.MapNode(BrainT3);
    BrainExtracellular.GetWaterVolumeFraction().SetValue(BrainEWFraction);
    BrainIntracellular.GetVolume().SetValue(BrainIWFraction * BrainTissueVolume * 1000.0, VolumeUnit::mL);
    BrainIntracellular.GetWaterVolumeFraction().SetValue(BrainIWFraction);
    BrainTissue.GetAcidicPhospohlipidConcentration().SetValue(BrainAPL, MassPerMassUnit::mg_Per_g);
    BrainTissue.GetNeutralLipidsVolumeFraction().SetValue(BrainNLFraction);
    BrainTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(BrainNPFraction);
    BrainTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(BrainAAGRatio);
    BrainTissue.GetTissueToPlasmaAlbuminRatio().SetValue(BrainARatio);
    BrainTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(BrainLRatio);
    BrainTissue.GetTotalMass().SetValue(BrainTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& BrainVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::Intracranial),
      BrainExtracellular, pulse::ExpandedVascularLink::BrainVascularToTissue);
    BrainVascularToTissue.MapPath(Intracranial1ToBrainT2);

    SELiquidCompartmentLink& BrainTissueToLymph = m_Compartments->CreateLiquidLink(BrainExtracellular, cLymph, pulse::LymphLink::BrainTissueToLymph);
    BrainTissueToLymph.MapPath(BrainT1ToLymph);

    /////////
    // Gut //
    SEFluidCircuitNode* SmallIntestine = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::SmallIntestine1);
    SEFluidCircuitNode* LargeIntestine = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LargeIntestine1);
    SEFluidCircuitNode* Splanchnic = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Splanchnic1);
    SEFluidCircuitNode& GutT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::GutT1);
    SEFluidCircuitNode& GutT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::GutT2);
    SEFluidCircuitNode& GutT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::GutT3);
    GutT1.GetPressure().Set(SmallIntestine->GetPressure());
    GutT3.GetPressure().Set(Ground->GetPressure());
    GutT1.GetVolumeBaseline().SetValue(GutEWFraction * GutTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& SmallIntestine1ToGutT2 = cCombinedCardiovascular.CreatePath(*SmallIntestine, GutT2, pulse::ExpandedCardiovascularTissuePath::SmallIntestine1ToGutT2);
    SmallIntestine1ToGutT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LargeIntestine1ToGutT2 = cCombinedCardiovascular.CreatePath(*LargeIntestine, GutT2, pulse::ExpandedCardiovascularTissuePath::LargeIntestine1ToGutT2);
    LargeIntestine1ToGutT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& Splanchnic1ToGutT2 = cCombinedCardiovascular.CreatePath(*Splanchnic, GutT2, pulse::ExpandedCardiovascularTissuePath::Splanchnic1ToGutT2);
    Splanchnic1ToGutT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& GutT2ToGutT1 = cCombinedCardiovascular.CreatePath(GutT2, GutT1, pulse::ExpandedCardiovascularTissuePath::GutT2ToGutT1);
    GutT2ToGutT1.GetResistanceBaseline().SetValue((1 / GutTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& GutT1ToGutT3 = cCombinedCardiovascular.CreatePath(GutT1, GutT3, pulse::ExpandedCardiovascularTissuePath::GutT1ToGutT3);
    GutT1ToGutT3.GetComplianceBaseline().SetValue(GutT1.GetVolumeBaseline(VolumeUnit::mL) / GutT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToGutT3 = cCombinedCardiovascular.CreatePath(*Ground, GutT3, pulse::ExpandedCardiovascularTissuePath::GroundToGutT3);
    GroundToGutT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& GutT1ToLymph = cCombinedCardiovascular.CreatePath(GutT1, Lymph, pulse::ExpandedCardiovascularTissuePath::GutT1ToLymph);
    GutT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& GutTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Gut);
    SELiquidCompartment& GutExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::GutExtracellular);
    SELiquidCompartment& GutIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::GutIntracellular);
    GutTissue.GetMatrixVolume().SetValue((1 - GutEWFraction - GutIWFraction) * GutTissueVolume * 1000.0, VolumeUnit::mL);
    GutExtracellular.MapNode(GutT1);
    GutExtracellular.MapNode(GutT2);
    GutExtracellular.MapNode(GutT3);
    GutExtracellular.GetWaterVolumeFraction().SetValue(GutEWFraction);
    GutIntracellular.GetVolume().SetValue(GutIWFraction * GutTissueVolume * 1000.0, VolumeUnit::mL);
    GutIntracellular.GetWaterVolumeFraction().SetValue(GutIWFraction);
    GutTissue.GetAcidicPhospohlipidConcentration().SetValue(GutAPL, MassPerMassUnit::mg_Per_g);
    GutTissue.GetNeutralLipidsVolumeFraction().SetValue(GutNLFraction);
    GutTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(GutNPFraction);
    GutTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(GutAAGRatio);
    GutTissue.GetTissueToPlasmaAlbuminRatio().SetValue(GutARatio);
    GutTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(GutLRatio);
    GutTissue.GetTotalMass().SetValue(GutTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& SmallIntestineVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::SmallIntestine),
      GutExtracellular, pulse::ExpandedVascularLink::SmallIntestineVascularToTissue);
    SmallIntestineVascularToTissue.MapPath(SmallIntestine1ToGutT2);

    SELiquidCompartmentLink& LargeIntestineVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LargeIntestine),
      GutExtracellular, pulse::ExpandedVascularLink::LargeIntestineVascularToTissue);
    LargeIntestineVascularToTissue.MapPath(LargeIntestine1ToGutT2);

    SELiquidCompartmentLink& SplanchnicVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::Splanchnic),
      GutExtracellular, pulse::ExpandedVascularLink::SplanchnicVascularToTissue);
    SplanchnicVascularToTissue.MapPath(Splanchnic1ToGutT2);

    SELiquidCompartmentLink& GutTissueToLymph = m_Compartments->CreateLiquidLink(GutExtracellular, cLymph, pulse::LymphLink::GutTissueToLymph);
    GutTissueToLymph.MapPath(GutT1ToLymph);

    /////////////////
    // Left Kidney //
    SEFluidCircuitNode* LeftKidney2;
    if (!m_Config->IsRenalEnabled())
      LeftKidney2 = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftKidney2);
    else
      LeftKidney2 = cCombinedCardiovascular.GetNode(pulse::RenalNode::LeftGlomerularCapillaries);

    SEFluidCircuitNode& LeftKidneyT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LeftKidneyT1);
    SEFluidCircuitNode& LeftKidneyT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LeftKidneyT2);
    SEFluidCircuitNode& LeftKidneyT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LeftKidneyT3);
    LeftKidneyT1.GetPressure().Set(LeftKidney2->GetPressure());
    LeftKidneyT3.GetPressure().Set(Ground->GetPressure());
    LeftKidneyT1.GetVolumeBaseline().SetValue(LKidneyEWFraction * LKidneyTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& LeftKidney2ToLeftKidneyT2 = cCombinedCardiovascular.CreatePath(*LeftKidney2, LeftKidneyT2, pulse::ExpandedCardiovascularTissuePath::LeftKidney2ToLeftKidneyT2);
    LeftKidney2ToLeftKidneyT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftKidneyT2ToLeftKidneyT1 = cCombinedCardiovascular.CreatePath(LeftKidneyT2, LeftKidneyT1, pulse::ExpandedCardiovascularTissuePath::LeftKidneyT2ToLeftKidneyT1);
    LeftKidneyT2ToLeftKidneyT1.GetResistanceBaseline().SetValue((1 / LKidneyTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftKidneyT1ToLeftKidneyT3 = cCombinedCardiovascular.CreatePath(LeftKidneyT1, LeftKidneyT3, pulse::ExpandedCardiovascularTissuePath::LeftKidneyT1ToLeftKidneyT3);
    LeftKidneyT1ToLeftKidneyT3.GetComplianceBaseline().SetValue(LeftKidneyT1.GetVolumeBaseline(VolumeUnit::mL) / LeftKidneyT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToLeftKidneyT3 = cCombinedCardiovascular.CreatePath(*Ground, LeftKidneyT3, pulse::ExpandedCardiovascularTissuePath::GroundToLeftKidneyT3);
    GroundToLeftKidneyT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& LeftKidneyT1ToLymph = cCombinedCardiovascular.CreatePath(LeftKidneyT1, Lymph, pulse::ExpandedCardiovascularTissuePath::LeftKidneyT1ToLymph);
    LeftKidneyT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& LeftKidneyTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::LeftKidney);
    SELiquidCompartment& LeftKidneyExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftKidneyExtracellular);
    SELiquidCompartment& LeftKidneyIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftKidneyIntracellular);
    LeftKidneyTissue.GetMatrixVolume().SetValue((1 - LKidneyEWFraction - LKidneyIWFraction) * LKidneyTissueVolume * 1000.0, VolumeUnit::mL);
    LeftKidneyExtracellular.MapNode(LeftKidneyT1);
    LeftKidneyExtracellular.MapNode(LeftKidneyT2);
    LeftKidneyExtracellular.MapNode(LeftKidneyT3);
    LeftKidneyExtracellular.GetWaterVolumeFraction().SetValue(LKidneyEWFraction);
    LeftKidneyIntracellular.GetVolume().SetValue(LKidneyIWFraction * LKidneyTissueVolume * 1000.0, VolumeUnit::mL);
    LeftKidneyIntracellular.GetWaterVolumeFraction().SetValue(LKidneyIWFraction);
    LeftKidneyTissue.GetAcidicPhospohlipidConcentration().SetValue(LKidneyAPL, MassPerMassUnit::mg_Per_g);
    LeftKidneyTissue.GetNeutralLipidsVolumeFraction().SetValue(LKidneyNLFraction);
    LeftKidneyTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(LKidneyNPFraction);
    LeftKidneyTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(LKidneyAAGRatio);
    LeftKidneyTissue.GetTissueToPlasmaAlbuminRatio().SetValue(LKidneyARatio);
    LeftKidneyTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(LKidneyLRatio);
    LeftKidneyTissue.GetTotalMass().SetValue(LKidneyTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& LeftKidneyVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftKidney),
      LeftKidneyExtracellular, pulse::ExpandedVascularLink::LeftKidneyVascularToTissue);
    LeftKidneyVascularToTissue.MapPath(LeftKidney2ToLeftKidneyT2);

    SELiquidCompartmentLink& LeftKidneyTissueToLymph = m_Compartments->CreateLiquidLink(LeftKidneyExtracellular, cLymph, pulse::LymphLink::LeftKidneyTissueToLymph);
    LeftKidneyTissueToLymph.MapPath(LeftKidneyT1ToLymph);

    ///////////////
    // Left Lung //
    SEFluidCircuitNode* LeftPulmonaryCapillaries1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftPulmonaryCapillaries1);
    SEFluidCircuitNode& LeftLungT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LeftLungT1);
    SEFluidCircuitNode& LeftLungT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LeftLungT2);
    SEFluidCircuitNode& LeftLungT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LeftLungT3);
    LeftLungT1.GetPressure().Set(LeftPulmonaryCapillaries1->GetPressure());
    LeftLungT3.GetPressure().Set(Ground->GetPressure());
    LeftLungT1.GetVolumeBaseline().SetValue(LLungEWFraction * LLungTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& LeftLung1ToLeftLungT2 = cCombinedCardiovascular.CreatePath(*LeftPulmonaryCapillaries1, LeftLungT2, pulse::ExpandedCardiovascularTissuePath::LeftLung1ToLeftLungT2);
    LeftLung1ToLeftLungT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftLungT2ToLeftLungT1 = cCombinedCardiovascular.CreatePath(LeftLungT2, LeftLungT1, pulse::ExpandedCardiovascularTissuePath::LeftLungT2ToLeftLungT1);
    LeftLungT2ToLeftLungT1.GetResistanceBaseline().SetValue((1 / LLungTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLungT1ToLeftLungT3 = cCombinedCardiovascular.CreatePath(LeftLungT1, LeftLungT3, pulse::ExpandedCardiovascularTissuePath::LeftLungT1ToLeftLungT3);
    LeftLungT1ToLeftLungT3.GetComplianceBaseline().SetValue(LeftLungT1.GetVolumeBaseline(VolumeUnit::mL) / LeftLungT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToLeftLungT3 = cCombinedCardiovascular.CreatePath(*Ground, LeftLungT3, pulse::ExpandedCardiovascularTissuePath::GroundToLeftLungT3);
    GroundToLeftLungT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& LeftLungT1ToLymph = cCombinedCardiovascular.CreatePath(LeftLungT1, Lymph, pulse::ExpandedCardiovascularTissuePath::LeftLungT1ToLymph);
    LeftLungT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& LeftLungTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::LeftLung);
    SELiquidCompartment& LeftLungExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftLungExtracellular);
    SELiquidCompartment& LeftLungIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftLungIntracellular);
    LeftLungTissue.GetMatrixVolume().SetValue((1 - LLungEWFraction - LLungIWFraction) * LLungTissueVolume * 1000.0, VolumeUnit::mL);
    LeftLungExtracellular.MapNode(LeftLungT1);
    LeftLungExtracellular.MapNode(LeftLungT2);
    LeftLungExtracellular.MapNode(LeftLungT3);
    LeftLungExtracellular.GetWaterVolumeFraction().SetValue(LLungEWFraction);
    LeftLungIntracellular.GetVolume().SetValue(LLungIWFraction * LLungTissueVolume * 1000.0, VolumeUnit::mL);
    LeftLungIntracellular.GetWaterVolumeFraction().SetValue(LLungIWFraction);
    LeftLungTissue.GetAcidicPhospohlipidConcentration().SetValue(LLungAPL, MassPerMassUnit::mg_Per_g);
    LeftLungTissue.GetNeutralLipidsVolumeFraction().SetValue(LLungNLFraction);
    LeftLungTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(LLungNPFraction);
    LeftLungTissue.GetTissueToPlasmaAlbuminRatio().SetValue(LLungARatio);
    LeftLungTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(LLungLRatio);
    LeftLungTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(LLungAAGRatio);
    LeftLungTissue.GetTotalMass().SetValue(LLungTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& LeftLungVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLung),
      LeftLungExtracellular, pulse::ExpandedVascularLink::LeftLungVascularToTissue);
    LeftLungVascularToTissue.MapPath(LeftLung1ToLeftLungT2);

    SELiquidCompartmentLink& LeftLungTissueToLymph = m_Compartments->CreateLiquidLink(LeftLungExtracellular, cLymph, pulse::LymphLink::LeftLungTissueToLymph);
    LeftLungTissueToLymph.MapPath(LeftLungT1ToLymph);

    ///////////
    // Liver //
    SEFluidCircuitNode* Liver1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Liver1);
    SEFluidCircuitNode& LiverT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LiverT1);
    SEFluidCircuitNode& LiverT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LiverT2);
    SEFluidCircuitNode& LiverT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::LiverT3);
    LiverT1.GetPressure().Set(Liver1->GetPressure());
    LiverT3.GetPressure().Set(Ground->GetPressure());
    LiverT1.GetVolumeBaseline().SetValue(LiverEWFraction * LiverTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& Liver1ToLiverT2 = cCombinedCardiovascular.CreatePath(*Liver1, LiverT2, pulse::ExpandedCardiovascularTissuePath::Liver1ToLiverT2);
    Liver1ToLiverT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LiverT2ToLiverT1 = cCombinedCardiovascular.CreatePath(LiverT2, LiverT1, pulse::ExpandedCardiovascularTissuePath::LiverT2ToLiverT1);
    LiverT2ToLiverT1.GetResistanceBaseline().SetValue((1 / LiverTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LiverT1ToLiverT3 = cCombinedCardiovascular.CreatePath(LiverT1, LiverT3, pulse::ExpandedCardiovascularTissuePath::LiverT1ToLiverT3);
    LiverT1ToLiverT3.GetComplianceBaseline().SetValue(LiverT1.GetVolumeBaseline(VolumeUnit::mL) / LiverT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToLiverT3 = cCombinedCardiovascular.CreatePath(*Ground, LiverT3, pulse::ExpandedCardiovascularTissuePath::GroundToLiverT3);
    GroundToLiverT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& LiverT1ToLymph = cCombinedCardiovascular.CreatePath(LiverT1, Lymph, pulse::ExpandedCardiovascularTissuePath::LiverT1ToLymph);
    LiverT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& LiverTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Liver);
    SELiquidCompartment& LiverExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::LiverExtracellular);
    SELiquidCompartment& LiverIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::LiverIntracellular);
    LiverTissue.GetMatrixVolume().SetValue((1 - LiverEWFraction - LiverIWFraction) * LiverTissueVolume * 1000.0, VolumeUnit::mL);
    LiverExtracellular.MapNode(LiverT1);
    LiverExtracellular.MapNode(LiverT2);
    LiverExtracellular.MapNode(LiverT3);
    LiverExtracellular.GetWaterVolumeFraction().SetValue(LiverEWFraction);
    LiverIntracellular.GetVolume().SetValue(LiverIWFraction * LiverTissueVolume * 1000.0, VolumeUnit::mL);
    LiverIntracellular.GetWaterVolumeFraction().SetValue(LiverIWFraction);
    LiverTissue.GetAcidicPhospohlipidConcentration().SetValue(LiverAPL, MassPerMassUnit::mg_Per_g);
    LiverTissue.GetNeutralLipidsVolumeFraction().SetValue(LiverNLFraction);
    LiverTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(LiverNPFraction);
    LiverTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(LiverAAGRatio);
    LiverTissue.GetTissueToPlasmaAlbuminRatio().SetValue(LiverARatio);
    LiverTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(LiverLRatio);
    LiverTissue.GetTotalMass().SetValue(LiverTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& LiverVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::Liver),
      LiverExtracellular, pulse::ExpandedVascularLink::LiverVascularToTissue);
    LiverVascularToTissue.MapPath(Liver1ToLiverT2);

    SELiquidCompartmentLink& LiverTissueToLymph = m_Compartments->CreateLiquidLink(LiverExtracellular, cLymph, pulse::LymphLink::LiverTissueToLymph);
    LiverTissueToLymph.MapPath(LiverT1ToLymph);

    ////////////
    // Muscle //
    SEFluidCircuitNode* TorsoMuscle1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::TorsoMuscle1);
    SEFluidCircuitNode* LeftArmMuscle1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftArmMuscle1);
    SEFluidCircuitNode* LeftLegMuscle1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftLegMuscle1);
    SEFluidCircuitNode* RightArmMuscle1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightArmMuscle1);
    SEFluidCircuitNode* RightLegMuscle1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightLegMuscle1);
    SEFluidCircuitNode& MuscleT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::MuscleT1);
    SEFluidCircuitNode& MuscleT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::MuscleT2);
    SEFluidCircuitNode& MuscleT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::MuscleT3);
    MuscleT1.GetPressure().Set(TorsoMuscle1->GetPressure());
    MuscleT3.GetPressure().Set(Ground->GetPressure());
    MuscleT1.GetVolumeBaseline().SetValue(MuscleEWFraction * MuscleTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& TorsoMuscle1ToMuscleT2 = cCombinedCardiovascular.CreatePath(*TorsoMuscle1, MuscleT2, pulse::ExpandedCardiovascularTissuePath::TorsoMuscle1ToMuscleT2);
    TorsoMuscle1ToMuscleT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftArmMuscle1ToMuscleT2 = cCombinedCardiovascular.CreatePath(*LeftArmMuscle1, MuscleT2, pulse::ExpandedCardiovascularTissuePath::LeftArmMuscle1ToMuscleT2);
    LeftArmMuscle1ToMuscleT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftLegMuscle1ToMuscleT2 = cCombinedCardiovascular.CreatePath(*LeftLegMuscle1, MuscleT2, pulse::ExpandedCardiovascularTissuePath::LeftLegMuscle1ToMuscleT2);
    LeftLegMuscle1ToMuscleT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightArmMuscle1ToMuscleT2 = cCombinedCardiovascular.CreatePath(*RightArmMuscle1, MuscleT2, pulse::ExpandedCardiovascularTissuePath::RightArmMuscle1ToMuscleT2);
    RightArmMuscle1ToMuscleT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightLegMuscle1ToMuscleT2 = cCombinedCardiovascular.CreatePath(*RightLegMuscle1, MuscleT2, pulse::ExpandedCardiovascularTissuePath::RightLegMuscle1ToMuscleT2);
    RightLegMuscle1ToMuscleT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& MuscleT2ToMuscleT1 = cCombinedCardiovascular.CreatePath(MuscleT2, MuscleT1, pulse::ExpandedCardiovascularTissuePath::MuscleT2ToMuscleT1);
    MuscleT2ToMuscleT1.GetResistanceBaseline().SetValue((1 / MuscleTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& MuscleT1ToMuscleT3 = cCombinedCardiovascular.CreatePath(MuscleT1, MuscleT3, pulse::ExpandedCardiovascularTissuePath::MuscleT1ToMuscleT3);
    MuscleT1ToMuscleT3.GetComplianceBaseline().SetValue(MuscleT1.GetVolumeBaseline(VolumeUnit::mL) / MuscleT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToMuscleT3 = cCombinedCardiovascular.CreatePath(*Ground, MuscleT3, pulse::ExpandedCardiovascularTissuePath::GroundToMuscleT3);
    GroundToMuscleT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& MuscleT1ToLymph = cCombinedCardiovascular.CreatePath(MuscleT1, Lymph, pulse::ExpandedCardiovascularTissuePath::MuscleT1ToLymph);
    MuscleT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& MuscleTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Muscle);
    SELiquidCompartment& MuscleExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::MuscleExtracellular);
    SELiquidCompartment& MuscleIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::MuscleIntracellular);
    MuscleTissue.GetMatrixVolume().SetValue((1 - MuscleEWFraction - MuscleIWFraction) * MuscleTissueVolume * 1000.0, VolumeUnit::mL);
    MuscleExtracellular.MapNode(MuscleT1);
    MuscleExtracellular.MapNode(MuscleT2);
    MuscleExtracellular.MapNode(MuscleT3);
    MuscleExtracellular.GetWaterVolumeFraction().SetValue(MuscleEWFraction);
    MuscleIntracellular.GetVolume().SetValue(MuscleIWFraction * MuscleTissueVolume * 1000.0, VolumeUnit::mL);
    MuscleIntracellular.GetWaterVolumeFraction().SetValue(MuscleIWFraction);
    MuscleTissue.GetAcidicPhospohlipidConcentration().SetValue(MuscleAPL, MassPerMassUnit::mg_Per_g);
    MuscleTissue.GetNeutralLipidsVolumeFraction().SetValue(MuscleNLFraction);
    MuscleTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(MuscleNPFraction);
    MuscleTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(MuscleAAGRatio);
    MuscleTissue.GetTissueToPlasmaAlbuminRatio().SetValue(MuscleARatio);
    MuscleTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(MuscleLRatio);
    MuscleTissue.GetTotalMass().SetValue(MuscleTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& TorsoMuscleVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoMuscle),
      MuscleExtracellular, pulse::ExpandedVascularLink::TorsoMuscleVascularToTissue);
    TorsoMuscleVascularToTissue.MapPath(TorsoMuscle1ToMuscleT2);
    SELiquidCompartmentLink& LeftArmMuscleVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmMuscle),
      MuscleExtracellular, pulse::ExpandedVascularLink::LeftArmMuscleVascularToTissue);
    LeftArmMuscleVascularToTissue.MapPath(LeftArmMuscle1ToMuscleT2);
    SELiquidCompartmentLink& LeftLegMuscleVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegMuscle),
      MuscleExtracellular, pulse::ExpandedVascularLink::LeftLegMuscleVascularToTissue);
    LeftLegMuscleVascularToTissue.MapPath(LeftLegMuscle1ToMuscleT2);
    SELiquidCompartmentLink& RightArmMuscleVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmMuscle),
      MuscleExtracellular, pulse::ExpandedVascularLink::RightArmMuscleVascularToTissue);
    RightArmMuscleVascularToTissue.MapPath(RightArmMuscle1ToMuscleT2);
    SELiquidCompartmentLink& RightLegMuscleVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegMuscle),
      MuscleExtracellular, pulse::ExpandedVascularLink::RightLegMuscleVascularToTissue);
    RightLegMuscleVascularToTissue.MapPath(RightLegMuscle1ToMuscleT2);

    SELiquidCompartmentLink& MuscleTissueToLymph = m_Compartments->CreateLiquidLink(MuscleExtracellular, cLymph, pulse::LymphLink::MuscleTissueToLymph);
    MuscleTissueToLymph.MapPath(MuscleT1ToLymph);

    ////////////////
    // Myocardium //
    SEFluidCircuitNode* Myocardium2 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Myocardium2);
    SEFluidCircuitNode& MyocardiumT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::MyocardiumT1);
    SEFluidCircuitNode& MyocardiumT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::MyocardiumT2);
    SEFluidCircuitNode& MyocardiumT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::MyocardiumT3);
    MyocardiumT1.GetPressure().Set(Myocardium2->GetPressure());
    MyocardiumT3.GetPressure().Set(Ground->GetPressure());
    MyocardiumT1.GetVolumeBaseline().SetValue(MyocardiumEWFraction * MyocardiumTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& Myocardium2ToMyocardiumT2 = cCombinedCardiovascular.CreatePath(*Myocardium2, MyocardiumT2, pulse::ExpandedCardiovascularTissuePath::Myocardium2ToMyocardiumT2);
    Myocardium2ToMyocardiumT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& MyocardiumT2ToMyocardiumT1 = cCombinedCardiovascular.CreatePath(MyocardiumT2, MyocardiumT1, pulse::ExpandedCardiovascularTissuePath::MyocardiumT2ToMyocardiumT1);
    MyocardiumT2ToMyocardiumT1.GetResistanceBaseline().SetValue((1 / MyocardiumTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& MyocardiumT1ToMyocardiumT3 = cCombinedCardiovascular.CreatePath(MyocardiumT1, MyocardiumT3, pulse::ExpandedCardiovascularTissuePath::MyocardiumT1ToMyocardiumT3);
    MyocardiumT1ToMyocardiumT3.GetComplianceBaseline().SetValue(MyocardiumT1.GetVolumeBaseline(VolumeUnit::mL) / MyocardiumT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToMyocardiumT3 = cCombinedCardiovascular.CreatePath(*Ground, MyocardiumT3, pulse::ExpandedCardiovascularTissuePath::GroundToMyocardiumT3);
    GroundToMyocardiumT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& MyocardiumT1ToLymph = cCombinedCardiovascular.CreatePath(MyocardiumT1, Lymph, pulse::ExpandedCardiovascularTissuePath::MyocardiumT1ToLymph);
    MyocardiumT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& MyocardiumTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Myocardium);
    SELiquidCompartment& MyocardiumExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::MyocardiumExtracellular);
    SELiquidCompartment& MyocardiumIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::MyocardiumIntracellular);
    MyocardiumTissue.GetMatrixVolume().SetValue((1 - MyocardiumEWFraction - MyocardiumIWFraction) * MyocardiumTissueVolume * 1000.0, VolumeUnit::mL);
    MyocardiumExtracellular.MapNode(MyocardiumT1);
    MyocardiumExtracellular.MapNode(MyocardiumT2);
    MyocardiumExtracellular.MapNode(MyocardiumT3);
    MyocardiumExtracellular.GetWaterVolumeFraction().SetValue(MyocardiumEWFraction);
    MyocardiumIntracellular.GetVolume().SetValue(MyocardiumIWFraction * MyocardiumTissueVolume * 1000.0, VolumeUnit::mL);
    MyocardiumIntracellular.GetWaterVolumeFraction().SetValue(MyocardiumIWFraction);
    MyocardiumTissue.GetAcidicPhospohlipidConcentration().SetValue(MyocardiumAPL, MassPerMassUnit::mg_Per_g);
    MyocardiumTissue.GetNeutralLipidsVolumeFraction().SetValue(MyocardiumNLFraction);
    MyocardiumTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(MyocardiumNPFraction);
    MyocardiumTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(MyocardiumAAGRatio);
    MyocardiumTissue.GetTissueToPlasmaAlbuminRatio().SetValue(MyocardiumARatio);
    MyocardiumTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(MyocardiumLRatio);
    MyocardiumTissue.GetTotalMass().SetValue(MyocardiumTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& MyocardiumVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::Myocardium),
      MyocardiumExtracellular, pulse::ExpandedVascularLink::MyocardiumVascularToTissue);
    MyocardiumVascularToTissue.MapPath(Myocardium2ToMyocardiumT2);

    SELiquidCompartmentLink& MyocardiumTissueToLymph = m_Compartments->CreateLiquidLink(MyocardiumExtracellular, cLymph, pulse::LymphLink::MyocardiumTissueToLymph);
    MyocardiumTissueToLymph.MapPath(MyocardiumT1ToLymph);

    //////////////////
    // Right Kidney //
    SEFluidCircuitNode* RightKidney2;
    if (!m_Config->IsRenalEnabled())
      RightKidney2 = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightKidney2);
    else
      RightKidney2 = cCombinedCardiovascular.GetNode(pulse::RenalNode::RightGlomerularCapillaries);

    SEFluidCircuitNode& RightKidneyT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::RightKidneyT1);
    SEFluidCircuitNode& RightKidneyT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::RightKidneyT2);
    SEFluidCircuitNode& RightKidneyT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::RightKidneyT3);
    RightKidneyT1.GetPressure().Set(RightKidney2->GetPressure());
    RightKidneyT3.GetPressure().Set(Ground->GetPressure());
    RightKidneyT1.GetVolumeBaseline().SetValue(RKidneyEWFraction * RKidneyTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& RightKidney2ToRightKidneyT2 = cCombinedCardiovascular.CreatePath(*RightKidney2, RightKidneyT2, pulse::ExpandedCardiovascularTissuePath::RightKidney2ToRightKidneyT2);
    RightKidney2ToRightKidneyT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightKidneyT2ToRightKidneyT1 = cCombinedCardiovascular.CreatePath(RightKidneyT2, RightKidneyT1, pulse::ExpandedCardiovascularTissuePath::RightKidneyT2ToRightKidneyT1);
    RightKidneyT2ToRightKidneyT1.GetResistanceBaseline().SetValue((1 / RKidneyTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightKidneyT1ToRightKidneyT3 = cCombinedCardiovascular.CreatePath(RightKidneyT1, RightKidneyT3, pulse::ExpandedCardiovascularTissuePath::RightKidneyT1ToRightKidneyT3);
    RightKidneyT1ToRightKidneyT3.GetComplianceBaseline().SetValue(RightKidneyT1.GetVolumeBaseline(VolumeUnit::mL) / RightKidneyT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToRightKidneyT3 = cCombinedCardiovascular.CreatePath(*Ground, RightKidneyT3, pulse::ExpandedCardiovascularTissuePath::GroundToRightKidneyT3);
    GroundToRightKidneyT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& RightKidneyT1ToLymph = cCombinedCardiovascular.CreatePath(RightKidneyT1, Lymph, pulse::ExpandedCardiovascularTissuePath::RightKidneyT1ToLymph);
    RightKidneyT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& RightKidneyTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::RightKidney);
    SELiquidCompartment& RightKidneyExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::RightKidneyExtracellular);
    SELiquidCompartment& RightKidneyIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::RightKidneyIntracellular);
    RightKidneyTissue.GetMatrixVolume().SetValue((1 - RKidneyEWFraction - RKidneyIWFraction) * RKidneyTissueVolume * 1000.0, VolumeUnit::mL);
    RightKidneyExtracellular.MapNode(RightKidneyT1);
    RightKidneyExtracellular.MapNode(RightKidneyT2);
    RightKidneyExtracellular.MapNode(RightKidneyT3);
    RightKidneyExtracellular.GetWaterVolumeFraction().SetValue(RKidneyEWFraction);
    RightKidneyIntracellular.GetVolume().SetValue(RKidneyIWFraction * RKidneyTissueVolume * 1000.0, VolumeUnit::mL);
    RightKidneyIntracellular.GetWaterVolumeFraction().SetValue(RKidneyIWFraction);
    RightKidneyTissue.GetAcidicPhospohlipidConcentration().SetValue(RKidneyAPL, MassPerMassUnit::mg_Per_g);
    RightKidneyTissue.GetNeutralLipidsVolumeFraction().SetValue(RKidneyNLFraction);
    RightKidneyTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(RKidneyNPFraction);
    RightKidneyTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(RKidneyAAGRatio);
    RightKidneyTissue.GetTissueToPlasmaAlbuminRatio().SetValue(RKidneyARatio);
    RightKidneyTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(RKidneyLRatio);
    RightKidneyTissue.GetTotalMass().SetValue(RKidneyTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& RightKidneyVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightKidney),
      RightKidneyExtracellular, pulse::ExpandedVascularLink::RightKidneyVascularToTissue);
    RightKidneyVascularToTissue.MapPath(RightKidney2ToRightKidneyT2);

    SELiquidCompartmentLink& RightKidneyTissueToLymph = m_Compartments->CreateLiquidLink(RightKidneyExtracellular, cLymph, pulse::LymphLink::RightKidneyTissueToLymph);
    RightKidneyTissueToLymph.MapPath(RightKidneyT1ToLymph);

    ////////////////
    // Right Lung //
    SEFluidCircuitNode* RightPulmonaryCapillaries1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightPulmonaryCapillaries1);
    SEFluidCircuitNode& RightLungT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::RightLungT1);
    SEFluidCircuitNode& RightLungT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::RightLungT2);
    SEFluidCircuitNode& RightLungT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::RightLungT3);
    RightLungT1.GetPressure().Set(RightPulmonaryCapillaries1->GetPressure());
    RightLungT3.GetPressure().Set(Ground->GetPressure());
    RightLungT1.GetVolumeBaseline().SetValue(RLungEWFraction * RLungTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& RightLung1ToRightLungT2 = cCombinedCardiovascular.CreatePath(*RightPulmonaryCapillaries1, RightLungT2, pulse::ExpandedCardiovascularTissuePath::RightLung1ToRightLungT2);
    RightLung1ToRightLungT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightLungT2ToRightLungT1 = cCombinedCardiovascular.CreatePath(RightLungT2, RightLungT1, pulse::ExpandedCardiovascularTissuePath::RightLungT2ToRightLungT1);
    RightLungT2ToRightLungT1.GetResistanceBaseline().SetValue((1 / RLungTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLungT1ToRightLungT3 = cCombinedCardiovascular.CreatePath(RightLungT1, RightLungT3, pulse::ExpandedCardiovascularTissuePath::RightLungT1ToRightLungT3);
    RightLungT1ToRightLungT3.GetComplianceBaseline().SetValue(RightLungT1.GetVolumeBaseline(VolumeUnit::mL) / RightLungT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToRightLungT3 = cCombinedCardiovascular.CreatePath(*Ground, RightLungT3, pulse::ExpandedCardiovascularTissuePath::GroundToRightLungT3);
    GroundToRightLungT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& RightLungT1ToLymph = cCombinedCardiovascular.CreatePath(RightLungT1, Lymph, pulse::ExpandedCardiovascularTissuePath::RightLungT1ToLymph);
    RightLungT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& RightLungTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::RightLung);
    SELiquidCompartment& RightLungExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::RightLungExtracellular);
    SELiquidCompartment& RightLungIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::RightLungIntracellular);
    RightLungTissue.GetMatrixVolume().SetValue((1 - RLungEWFraction - RLungIWFraction) * RLungTissueVolume * 1000.0, VolumeUnit::mL);
    RightLungExtracellular.MapNode(RightLungT1);
    RightLungExtracellular.MapNode(RightLungT2);
    RightLungExtracellular.MapNode(RightLungT3);
    RightLungExtracellular.GetWaterVolumeFraction().SetValue(RLungEWFraction);
    RightLungIntracellular.GetVolume().SetValue(RLungIWFraction * RLungTissueVolume * 1000.0, VolumeUnit::mL);
    RightLungIntracellular.GetWaterVolumeFraction().SetValue(RLungIWFraction);
    RightLungTissue.GetAcidicPhospohlipidConcentration().SetValue(RLungAPL, MassPerMassUnit::mg_Per_g);
    RightLungTissue.GetNeutralLipidsVolumeFraction().SetValue(RLungNLFraction);
    RightLungTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(RLungNPFraction);
    RightLungTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(RLungAAGRatio);
    RightLungTissue.GetTissueToPlasmaAlbuminRatio().SetValue(RLungARatio);
    RightLungTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(RLungLRatio);
    RightLungTissue.GetTotalMass().SetValue(RLungTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& RightLungVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLung),
      RightLungExtracellular, pulse::ExpandedVascularLink::RightLungVascularToTissue);
    RightLungVascularToTissue.MapPath(RightLung1ToRightLungT2);

    SELiquidCompartmentLink& RightLungTissueToLymph = m_Compartments->CreateLiquidLink(RightLungExtracellular, cLymph, pulse::LymphLink::RightLungTissueToLymph);
    RightLungTissueToLymph.MapPath(RightLungT1ToLymph);

    //////////
    // Skin //
    SEFluidCircuitNode* TorsoSkin1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::TorsoSkin1);
    SEFluidCircuitNode* LeftArmSkin1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftArmSkin1);
    SEFluidCircuitNode* LeftLegSkin1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::LeftLegSkin1);
    SEFluidCircuitNode* RightArmSkin1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightArmSkin1);
    SEFluidCircuitNode* RightLegSkin1 = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::RightLegSkin1);
    SEFluidCircuitNode& SkinT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::SkinT1);
    SEFluidCircuitNode& SkinT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::SkinT2);
    SEFluidCircuitNode& SkinT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::SkinT3);
    SkinT1.GetPressure().Set(TorsoSkin1->GetPressure());
    SkinT3.GetPressure().Set(Ground->GetPressure());
    SkinT1.GetVolumeBaseline().SetValue(SkinEWFraction * SkinTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& TorsoSkin1ToSkinT2 = cCombinedCardiovascular.CreatePath(*TorsoSkin1, SkinT2, pulse::ExpandedCardiovascularTissuePath::TorsoSkin1ToSkinT2);
    TorsoSkin1ToSkinT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftArmSkin1ToSkinT2 = cCombinedCardiovascular.CreatePath(*LeftArmSkin1, SkinT2, pulse::ExpandedCardiovascularTissuePath::LeftArmSkin1ToSkinT2);
    LeftArmSkin1ToSkinT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftLegSkin1ToSkinT2 = cCombinedCardiovascular.CreatePath(*LeftLegSkin1, SkinT2, pulse::ExpandedCardiovascularTissuePath::LeftLegSkin1ToSkinT2);
    LeftLegSkin1ToSkinT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightArmSkin1ToSkinT2 = cCombinedCardiovascular.CreatePath(*RightArmSkin1, SkinT2, pulse::ExpandedCardiovascularTissuePath::RightArmSkin1ToSkinT2);
    RightArmSkin1ToSkinT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& RightLegSkin1ToSkinT2 = cCombinedCardiovascular.CreatePath(*RightLegSkin1, SkinT2, pulse::ExpandedCardiovascularTissuePath::RightLegSkin1ToSkinT2);
    RightLegSkin1ToSkinT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& SkinT2ToSkinT1 = cCombinedCardiovascular.CreatePath(SkinT2, SkinT1, pulse::ExpandedCardiovascularTissuePath::SkinT2ToSkinT1);
    SkinT2ToSkinT1.GetResistanceBaseline().SetValue((1 / SkinTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& SkinT1ToSkinT3 = cCombinedCardiovascular.CreatePath(SkinT1, SkinT3, pulse::ExpandedCardiovascularTissuePath::SkinT1ToSkinT3);
    SkinT1ToSkinT3.GetComplianceBaseline().SetValue(SkinT1.GetVolumeBaseline(VolumeUnit::mL) / SkinT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToSkinT3 = cCombinedCardiovascular.CreatePath(*Ground, SkinT3, pulse::ExpandedCardiovascularTissuePath::GroundToSkinT3);
    GroundToSkinT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& SkinT1ToLymph = cCombinedCardiovascular.CreatePath(SkinT1, Lymph, pulse::ExpandedCardiovascularTissuePath::SkinT1ToLymph);
    SkinT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SEFluidCircuitPath& SkinT1ToGround = cCombinedCardiovascular.CreatePath(SkinT1, *Ground, pulse::ExpandedCardiovascularTissuePath::SkinSweating);
    SkinT1ToGround.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& SkinTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Skin);
    SELiquidCompartment& SkinExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::SkinExtracellular);
    SELiquidCompartment& SkinIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::SkinIntracellular);
    SkinTissue.GetMatrixVolume().SetValue((1 - SkinEWFraction - SkinIWFraction) * SkinTissueVolume * 1000.0, VolumeUnit::mL);
    SkinExtracellular.MapNode(SkinT1);
    SkinExtracellular.MapNode(SkinT2);
    SkinExtracellular.MapNode(SkinT3);
    SkinExtracellular.GetWaterVolumeFraction().SetValue(SkinEWFraction);
    SkinIntracellular.GetVolume().SetValue(SkinIWFraction * SkinTissueVolume * 1000.0, VolumeUnit::mL);
    SkinIntracellular.GetWaterVolumeFraction().SetValue(SkinIWFraction);
    SkinTissue.GetAcidicPhospohlipidConcentration().SetValue(SkinAPL, MassPerMassUnit::mg_Per_g);
    SkinTissue.GetNeutralLipidsVolumeFraction().SetValue(SkinNLFraction);
    SkinTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(SkinNPFraction);
    SkinTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(SkinAAGRatio);
    SkinTissue.GetTissueToPlasmaAlbuminRatio().SetValue(SkinARatio);
    SkinTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(SkinLRatio);
    SkinTissue.GetTotalMass().SetValue(SkinTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& TorsoSkinVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoSkin),
      SkinExtracellular, pulse::ExpandedVascularLink::TorsoSkinVascularToTissue);
    TorsoSkinVascularToTissue.MapPath(TorsoSkin1ToSkinT2);
    SELiquidCompartmentLink& LeftArmSkinVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmSkin),
      SkinExtracellular, pulse::ExpandedVascularLink::LeftArmSkinVascularToTissue);
    LeftArmSkinVascularToTissue.MapPath(LeftArmSkin1ToSkinT2);
    SELiquidCompartmentLink& LeftLegSkinVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegSkin),
      SkinExtracellular, pulse::ExpandedVascularLink::LeftLegSkinVascularToTissue);
    LeftLegSkinVascularToTissue.MapPath(LeftLegSkin1ToSkinT2);
    SELiquidCompartmentLink& RightArmSkinVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmSkin),
      SkinExtracellular, pulse::ExpandedVascularLink::RightArmSkinVascularToTissue);
    RightArmSkinVascularToTissue.MapPath(RightArmSkin1ToSkinT2);
    SELiquidCompartmentLink& RightLegSkinVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegSkin),
      SkinExtracellular, pulse::ExpandedVascularLink::RightLegSkinVascularToTissue);
    RightLegSkinVascularToTissue.MapPath(RightLegSkin1ToSkinT2);

    SELiquidCompartmentLink& SkinTissueToLymph = m_Compartments->CreateLiquidLink(SkinExtracellular, cLymph, pulse::LymphLink::SkinTissueToLymph);
    SkinTissueToLymph.MapPath(SkinT1ToLymph);

    ////////////
    // Spleen //
    SEFluidCircuitNode* Spleen = cCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Spleen1);
    SEFluidCircuitNode& SpleenT1 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::SpleenT1);
    SEFluidCircuitNode& SpleenT2 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::SpleenT2);
    SEFluidCircuitNode& SpleenT3 = cCombinedCardiovascular.CreateNode(pulse::TissueNode::SpleenT3);
    SpleenT1.GetPressure().Set(Spleen->GetPressure());
    SpleenT3.GetPressure().Set(Ground->GetPressure());
    SpleenT1.GetVolumeBaseline().SetValue(SpleenEWFraction * SpleenTissueVolume * 1000.0, VolumeUnit::mL);

    SEFluidCircuitPath& Spleen1ToSpleenT2 = cCombinedCardiovascular.CreatePath(*Spleen, SpleenT2, pulse::ExpandedCardiovascularTissuePath::Spleen1ToSpleenT2);
    Spleen1ToSpleenT2.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& SpleenT2ToSpleenT1 = cCombinedCardiovascular.CreatePath(SpleenT2, SpleenT1, pulse::ExpandedCardiovascularTissuePath::SpleenT2ToSpleenT1);
    SpleenT2ToSpleenT1.GetResistanceBaseline().SetValue((1 / SpleenTissueMass) * resistanceConstant, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& SpleenT1ToSpleenT3 = cCombinedCardiovascular.CreatePath(SpleenT1, SpleenT3, pulse::ExpandedCardiovascularTissuePath::SpleenT1ToSpleenT3);
    SpleenT1ToSpleenT3.GetComplianceBaseline().SetValue(SpleenT1.GetVolumeBaseline(VolumeUnit::mL) / SpleenT1.GetPressure(PressureUnit::mmHg), VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& GroundToSpleenT3 = cCombinedCardiovascular.CreatePath(*Ground, SpleenT3, pulse::ExpandedCardiovascularTissuePath::GroundToSpleenT3);
    GroundToSpleenT3.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& SpleenT1ToLymph = cCombinedCardiovascular.CreatePath(SpleenT1, Lymph, pulse::ExpandedCardiovascularTissuePath::SpleenT1ToLymph);
    SpleenT1ToLymph.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SETissueCompartment& SpleenTissue = m_Compartments->CreateTissueCompartment(pulse::TissueCompartment::Spleen);
    SELiquidCompartment& SpleenExtracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::SpleenExtracellular);
    SELiquidCompartment& SpleenIntracellular = m_Compartments->CreateLiquidCompartment(pulse::ExtravascularCompartment::SpleenIntracellular);
    SpleenTissue.GetMatrixVolume().SetValue((1 - SpleenEWFraction - SpleenIWFraction) * SpleenTissueVolume * 1000.0, VolumeUnit::mL);
    SpleenExtracellular.MapNode(SpleenT1);
    SpleenExtracellular.MapNode(SpleenT2);
    SpleenExtracellular.MapNode(SpleenT3);
    SpleenExtracellular.GetWaterVolumeFraction().SetValue(SpleenEWFraction);
    SpleenIntracellular.GetVolume().SetValue(SpleenIWFraction * SpleenTissueVolume * 1000.0, VolumeUnit::mL);
    SpleenIntracellular.GetWaterVolumeFraction().SetValue(SpleenIWFraction);
    SpleenTissue.GetAcidicPhospohlipidConcentration().SetValue(SpleenAPL, MassPerMassUnit::mg_Per_g);
    SpleenTissue.GetNeutralLipidsVolumeFraction().SetValue(SpleenNLFraction);
    SpleenTissue.GetNeutralPhospholipidsVolumeFraction().SetValue(SpleenNPFraction);
    SpleenTissue.GetTissueToPlasmaAlphaAcidGlycoproteinRatio().SetValue(SpleenAAGRatio);
    SpleenTissue.GetTissueToPlasmaAlbuminRatio().SetValue(SpleenARatio);
    SpleenTissue.GetTissueToPlasmaLipoproteinRatio().SetValue(SpleenLRatio);
    SpleenTissue.GetTotalMass().SetValue(SpleenTissueMass, MassUnit::kg);

    SELiquidCompartmentLink& SpleenVascularToTissue = m_Compartments->CreateLiquidLink(*m_Compartments->GetLiquidCompartment(pulse::ExpandedVascularCompartment::Spleen),
      SpleenExtracellular, pulse::ExpandedVascularLink::SpleenVascularToTissue);
    SpleenVascularToTissue.MapPath(Spleen1ToSpleenT2);

    SELiquidCompartmentLink& SpleenTissueToLymph = m_Compartments->CreateLiquidLink(SpleenExtracellular, cLymph, pulse::LymphLink::SpleenTissueToLymph);
    SpleenTissueToLymph.MapPath(SpleenT1ToLymph);

    cCombinedCardiovascular.SetNextAndCurrentFromBaselines();
    cCombinedCardiovascular.StateChange();

  }

  void Controller::SetupExpandedCardiovascularCerebrospinalFluid()
  {
    Info("Setting Up Expanded Cardiovascular Cerebrospinal Fluid");
    SEFluidCircuit& cCombinedCardiovascular = m_Circuits->GetActiveCardiovascularCircuit();

    SEFluidCircuitNode* Ground = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Ground);
    SEFluidCircuitNode* Intracranial2 = cCombinedCardiovascular.GetNode(pulse::ExpandedCardiovascularNode::Intracranial2);

    double brainVascularCompliance = cCombinedCardiovascular.GetPath(pulse::ExpandedCardiovascularPath::Intracranial2ToGround)->GetComplianceBaseline().GetValue(VolumePerPressureUnit::mL_Per_mmHg);
    double brainVascularPressure = Intracranial2->GetPressure().GetValue(PressureUnit::mmHg);

    m_Circuits->DeleteFluidPath(pulse::ExpandedCardiovascularPath::Intracranial2ToGround);

    SEFluidCircuitNode& VascularCSFBarrier = cCombinedCardiovascular.CreateNode(pulse::CerebrospinalFluidNode::VascularCSFBarrier);
    VascularCSFBarrier.GetPressure().SetValue(brainVascularPressure, PressureUnit::mmHg);

    //Initialize these values based on height/weight
    SEFluidCircuitNode& IntracranialSpace1 = cCombinedCardiovascular.CreateNode(pulse::CerebrospinalFluidNode::IntracranialSpace1);
    IntracranialSpace1.GetPressure().SetValue(7.0, PressureUnit::mmHg);
    IntracranialSpace1.GetVolumeBaseline().SetValue(0.0, VolumeUnit::mL);
    SEFluidCircuitNode& IntracranialSpace2 = cCombinedCardiovascular.CreateNode(pulse::CerebrospinalFluidNode::IntracranialSpace2);
    IntracranialSpace2.GetPressure().SetValue(7.0, PressureUnit::mmHg);
    IntracranialSpace2.GetVolumeBaseline().SetValue(100.0, VolumeUnit::mL);

    SEFluidCircuitPath& Brain1ToVascularCSFBarrier = cCombinedCardiovascular.CreatePath(*Intracranial2, VascularCSFBarrier, pulse::CerebrospinalFluidPath::Brain1ToVascularCSFBarrier);
    Brain1ToVascularCSFBarrier.GetComplianceBaseline().SetValue(brainVascularCompliance, VolumePerPressureUnit::mL_Per_mmHg); //Vascular Volume

    cCombinedCardiovascular.CreatePath(VascularCSFBarrier, IntracranialSpace1, pulse::CerebrospinalFluidPath::VascularCSFBarrierToIntracranialSpace1);

    SEFluidCircuitPath& GroundToIntracranialSpace1 = cCombinedCardiovascular.CreatePath(*Ground, IntracranialSpace1, pulse::CerebrospinalFluidPath::GroundToIntracranialSpace1);
    GroundToIntracranialSpace1.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);  //Absorption/Production Path

    SEFluidCircuitPath& IntracranialSpace1ToIntracranialSpace2 = cCombinedCardiovascular.CreatePath(IntracranialSpace1, IntracranialSpace2, pulse::CerebrospinalFluidPath::IntracranialSpace1ToIntracranialSpace2);
    IntracranialSpace1ToIntracranialSpace2.GetComplianceBaseline().SetValue(70.0, VolumePerPressureUnit::mL_Per_mmHg); //CSF Volume

    cCombinedCardiovascular.CreatePath(IntracranialSpace2, *Ground, pulse::CerebrospinalFluidPath::IntracranialSpace2ToGround);

    cCombinedCardiovascular.SetNextAndCurrentFromBaselines();
    cCombinedCardiovascular.StateChange();

    // Compartment
    SELiquidCompartment& cIntracranialSpace = m_Compartments->CreateLiquidCompartment(pulse::CerebrospinalFluidCompartment::IntracranialSpace);
    cIntracranialSpace.MapNode(IntracranialSpace1);
    cIntracranialSpace.MapNode(IntracranialSpace2);

    SELiquidCompartmentGraph& gCombinedCardiovascular = m_Compartments->GetActiveCardiovascularGraph();
    gCombinedCardiovascular.AddCompartment(cIntracranialSpace);
    gCombinedCardiovascular.StateChange();

  }
}