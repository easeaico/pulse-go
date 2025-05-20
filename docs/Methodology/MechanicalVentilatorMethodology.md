Mechanical Ventilator Methodology {#MechanicalVentilatorMethodology}
==========================

@anchor ventilator-overview
# Overview
@anchor ventilator-abstract
## Abstract

We have designed and implemented a generic virtual mechanical ventilator model into Pulse for real-time medical simulation.  The universal data model is uniquely designed to apply all modes of ventilation and allow for modification of the fluid mechanics circuit parameters.  The ventilator methodology provides a connection to the existing Pulse respiratory system for spontaneous breathing and gas/aerosol substance transport. The existing Pulse Explorer application was extended to include a new ventilator monitor screen with variable modes and settings and a dynamic output display.  Proper functionality was validated by simulating the same patient pathophysiology and ventilator settings virtually in Pulse as a physical lung simulator and ventilator setup. 

@anchor ventilator-intro
## Introduction

### Mechanical Ventilators and Positive-Pressure Ventilation

Mechanical ventilators use mechanical means for artificial ventilation to assist or replace spontaneous breathing.  Positive pressure ventilation pushes air into the lungs through the airways.  Ventilators provide the following benefits @cite cleveland2020mechanical :
- The patient does not have to work as hard to breathe - their respiratory muscles rest.
- The patient's as allowed time to recover in hopes that breathing becomes normal again.
- Helps the patient get adequate oxygen and clears carbon dioxide.
- Preserves a stable airway and preventing injury from aspiration.

### Taxonomy for Mechanical Ventilation

The terminology of ventilation modes is not globally standardized. Ventilator manufacturers and researchers name new modes even for trivial changes or innovations @cite lei2017medical. The consequence is a chaotic pool of mode terminology, where the number of modes of ventilation has grown exponentially in the last three decades. Existing respiratory care equipment includes hundreds of unique names of modes on more than 30 different ventilators. To meet the needs of a Pulse mechanical ventilator model, all existing modes of ventilation were generalized to an accurate and manageable set of configuration parameters. This requires an understanding of the base taxonomy of mode definitions used by respiratory care professionals. Previous work has been done to classified modes of ventilation through 10 fundamental constructs of ventilator technology in order to define the three basic components @cite chatburn2014taxonomy : (1) ventilator breath control variable, (2) the breath sequence, and (3) the targeting scheme. We have leveraged this theoretical basis of taxonomy and standard ontologies @cite carlo2012classification for our definitions of ventilator settings that are exposed to Pulse users and applications.

### Requirements

A robust Pulse mechanical ventilator model is needed for several different use cases, including clinical investigations, training simulators, and hardware-in-the-loop applications. This new ventilator model required a generic representation of a positive-pressure ventilation device with functionality for inhaled gas/agent administration. It needed to integrate with the existing Pulse respiratory fluid mechanics circuit and transport graph using the same physics-based fidelity previously implemented with legacy equipment models. Therefore, a mechanistic modeling approach was employed to allow all existing Pulse functionality for patient-specific physiologies and pathophysiologies to be simulated with the addition of artificial ventilation. Feedback between the respiratory system and the mechanical ventilator was required to trigger assisted modes from patient spontaneous breaths @cite 2021YartsevTriggering , while still allowing for equipment initiated modes of ventilation. The new Pulse mechanical ventilator methodology also needed to be modular to allow the simulation of all conventional and adaptive ventilation modes @cite lei2017medical, as well as various equipment brands and models with disparate definitions for settings, sensors, and interfaces.  Extensibility was required to interface with real-world cyber-physical systems, including prototype equipment for in silico trials.

While other ventilator virtual software and computational models exist @cite hannon2022modeling @cite lino2016critical, none are suitable for direct integration into Pulse. Existing software is often hardcoded with other physiology models and not modular enough for extraction, not realistic/high-fidelity enough for integration with Pulse's lumped parameter approach, too focused on specific modes and pathophysiological states, and/or is closed-source without a permissive license. Therefore, a new approach was required.

@anchor ventilator-system
# System Design

## Features and Capabilities

### Circuit

The mechanical ventilator model consists of a pressure/flow source with tubes and valves for inspiration and expiration. The unidirectional valves are ideal and do not allow any backflow. Figure 1 shows the mechanical ventilator circuit. The compartments and transport graph mirrors the circuit. Substance values are set on the ventilator node/compartment, assuming infinite volume. The ventilator interacts with the existing respiratory circuit through a direct connection that allows air (gases and aerosols) to flow freely. A path connects the airway node of the respiratory system to the connection (mask or tube) node of the ventilator. When the machine is turned on, both individually defined circuits are combined into a single, closed-loop circuit that is solved as a single linear algebra matrix.

@htmlonly
<center><a href="./Images/MechanicalVentilator/MechanicalVentilatorCircuit.png"><img src="./Images/MechanicalVentilator/MechanicalVentilatorCircuit.png" style="width:75%;"></a></center>
@endhtmlonly
<center>
<i>Figure 1. The Pulse 0-D mechanical ventilator fluid circuit diagram with an example pressure source waveform. Node objects hold volumes and pressures, while paths hold element definitions and flows.  The circuit employs a variable driver source (either pressure or flow), resistances, valves, and a compliance. The example driver waveform shown has a ramp waveform function for inspiration and square for expiration. The bracketed numbers and letters correspond to the enumerations previously listed in the Settings section.</i>
</center><br>

### Connecting to the Respiratory Circuit

When an mechanical ventilator is used on a patient, there is a direct
connection that allows air to flow freely between both. In the same
fashion, the Mechanical Ventilator and %Respiratory circuits in the engine are
directly connected and allowed to share the same fluid. When the machine
is turned on, both individually defined circuits are combined into a
single circuit that is then used for calculations.

@anchor ventilator-settings
### Settings

We have designed the ventilator to integrate with the existing CDM/API architectures using these Pulse software framework features. The goal of the ventilator data model is to distill all possible mode taxonomies into a minimal, generalized set of configuration parameters that can be parsed by the internal engine methodology. To achieve this, the settings do not include mode definitions specifically, but are instead a logical layer that defines the lower level parameters necessary for breath phase definitions. Much of the mechanical ventilator data model is focused on the calculation of the instantaneous pressure/flow source required to drive the circuit (see 2-4 below). These temporal functions are an important part of the transient fluid mechanics analysis used in Pulse simulations.  Each time-step the ventilator model must update circuit parameters to their proper value based on the current phase in the breath cycle and configuration settings.  Other configuration parameters define the fluid mechanics circuit elements (see 1 and 6-9 below) and values for the substance transport graph (see 5 below).

The Mechanical Ventilator configuration parameters were defined to allow for setting any ventilation modes - all control variable types and all breath sequences. While these definitions are intended to be all-encompassing, the data model has been implemented to allow for the addition of new features in the future.  Logical checks are included to ensure minimal required settings are defined and conflicting values are handled. The hierarchical configuration definitions are:

- 1. Connection: Connection type to the respiratory system (Off, Mask, Tube)
- 2. Inspiration Phase
    - (a) Cycle: Transition to pause/expiration
        - i. Machine Cycle: Control cycles
            - A. Time: Total length of inspiration phase to trigger expiration phase
        - ii. Patient Cycle: Assisted cycles
            - A. Pressure: Ventilator sensor pressure value to trigger expiration phase
            - B. Flow: Ventilator sensor flow value to trigger expiration phase
            - C. Volume: Ventilator sensor volume change (tidal volume) to trigger expiration phase
            - D. Respiratory Model: Trigger expiration phase from respiratory model event
    - (b) Waveform: Pattern of driver function (square, exponential, ramp, sinusoidal, sigmoidal)
    - (c) Waveform Period: Time to reach maximum driver value
    - (d) Target: Driver value set-point
        - i. Pressure (PIP): Extrinsic pressure above atmosphere at the end of inhalation
        - ii. Flow: Air flow at end of inhalation
    - (e) Limit: Cutoff/Maximum
        - i. Pressure: Ventilator sensor pressure cutoff/maximum
        - ii. Flow: Ventilator sensor flow cutoff/maximum
        - iii. Volume: Ventilator sensor total lung volume cutoff/maximum
    - (f) Pause: Time of plateau (i.e., constant driver pressure) between inspiration and expiration
- 3. Expiration Phase
    - (a) Trigger: Transition to inspiration
        - i. Machine Trigger: Control triggers
            - A. Time: Total length of expiration phase to trigger inspiration phase
        - ii. Patient Trigger: Assisted triggers
            - A. Pressure: Ventilator sensor pressure value to trigger inspiration phase
            - B. Volume: Ventilator sensor volume change value to trigger inspiration phase
            - C. Respiratory Model: Trigger inspiration phase from respiratory model event
    - (b) Waveform: Pattern of driver function (square, exponential, ramp, sinusoidal, sigmoidal)
    - (c) Waveform Period: Time to reach minimum driver value
    - (d) Baseline: Value to set/achieve
        - i. Pressure (PEEP): Extrinsic pressure above atmosphere at the end of exhalation
        - ii. Volume (FRC): Total lung volume at the end of exhalation
- 4. Driver Damping Parameter: Fractional change parameter to prevent abrupt changes and discontinuities (i.e., smoother driver curve)
- 5. Substances
    - (a) Fraction of inspired gas: FiO2 and other gases fractions
    - (b) Concentration of inspired aerosol: albuterol, etc.
- 6. Circuit Resistances
    - (a) Inspiration tube resistance: Total resistance of inspiratory limb tubing
    - (b) Inspiration valve resistance: Total resistance of inspiratory valves    
    - (c) Expiration tube resistance: Total resistance of expiratory limb tubing
    - (d) Expiration valve resistance: Total resistance of expiratory valves
- 7. Circuit Volumes
    - (a) Inspiratory limb volume: Total volume of inspiratory limb tubing
    - (b) Inspiratory valve volume: Total volume of inspiratory valves
    - (c) Expiratory tube volume: Total volume of expiratory limb tubing
    - (d) Expiratory valve volume: Total volume of expiratory valves
    - (e) Y-piece volume: Total volume of the y-piece
    - (f) Connection volume: Total volume of the mask/endotracheal tube
- 8. Circuit Compliance: Total compliance of the entire ventilator circuit
- 9. Relief Valve Pressure: Maximum relative pressure allowed

While the parameter list is meant to be all encompassing for all ventilator modes, some typical settings are required to be translated by the user or application.  For example, the respiration rate (<i>RR</i>) and I:E Ratio (<i>IE</i>)settings can be translated to an inspiratory period (<i>P<sub>ins</sub></i>) and expiratory period (<i>P<sub>exp</sub></i>) by:

\f[{P_{tot}}[s] = \frac{{60}}{{RR[bpm]}}\f]
<center>
<i>Equation 1.</i>
</center><br> 

\f[{P_{ins}}[s] = \frac{{IE \cdot {P_{tot}}[s]}}{{1 + IE}}\f]
<center>
<i>Equation 2.</i>
</center><br> 

\f[{P_{exp}}[s] = {P_{tot}}[s] - {P_{ins}}[s]\f]
<center>
<i>Equation 3.</i>
</center><br> 

Where the I:E Ratio (<i>IE</i>) is defined by a fraction, for example 1:2 is 0.5 and 1:1 is 1.0.

Standard ventilator mode settings have been added to allow for more intuitive application of ventilator modes. The following mode settings are automatically translated to the lower level configuration settings that are listed previously in this section.  Some setting are not exposed (e.g., trigger flow and cycle flow) at this level and default values are used.

Pressure Control (PC-CMV and PC-AC):
- Fraction of Inspired Oxygen
- Inspiratory Period
- Inspiratory Pressure
- Positive End Expired Pressure
- Respiration Rate
- Slope

Volume Control (VC-CMV and VC-AC):
- Flow
- Fraction of Inspired Oxygen
- Inspiratory Period (optional - if not set, transition to exhale when Tidal Volume achieved)
- Positive End Expired Pressure
- Respiration Rate
- Tidal Volume

Continuous Positive Airway Pressure (CPAP):
- Delta Pressure Support
- Fraction of Inspired Oxygen
- Positive End Expired Pressure
- Slope

@anchor ventilator-actions
### Actions

The ventilator model includes the application of leak and hold actions.

The leak severity is translated to the connection to environment resistance to allow more or less air to escape.

The hold action can be set to be applied instantaneously, at the end of the expiration cycle, or at the end of the inspiration cycle.  When the hold is active, the driver will be set to a zero flow source.  The hold will remain applied until explicitly removed by the user.

### Data Flow

The mechanical ventilator model follows the same logic loop as all Pulse systems, as shown in Figure 2. The state at every time-step is determined through a three-step process: 1) a preprocess step determines the circuit element values based on feedback mechanisms and engine settings/actions, 2) a process step uses the generic circuit calculator and substance transporter to compute the entire state of the circuit and fill in all pertinent values, and 3) a postprocess step is used to advance time. 

The software logic we implemented is meant to mimic the control software of real-world ventilators.  Feedback is read from locations within the ventilator circuit as if they are from actual sensors.  The data model was used to generate a data flow that is distilled down to simple conditionals. The model modifies the ventilator circuit elements each time-step based on the configuration settings and actions and the Pulse circuit solver and transporter handle the fluid mechanics and substance computations.

@htmlonly
<center><a href="./Images/MechanicalVentilator/BreathFlowDiagram.png"><img src="./Images/MechanicalVentilator/BreathFlowDiagram.png" style="width:75%;"></a></center>
@endhtmlonly
<center>
<i>Figure 2. We implemented a phased breath logic in the Pulse mechanical ventilator methodology. The bracketed numbers and letters correspond to the enumerations previously listed in the Settings section. Blue boxes signify transitions between the two phases that make up one full breath, white diamonds are conditionals, green boxes are the pressure/flow driver updates that occur each time-step, and grey boxes advance the simulation time to the next time-step. The ventilator can be stopped at any time-step during simulation.</i>
</center><br>

@anchor ventilator-dependencies
### Dependencies

The Mechanical Ventilator interacts with 
the %Respiratory System
through a connection that delivers gases and aerosols into the
%Respiratory System (see @ref RespiratoryMethodology). The two 
systems are connected to each other through a path that connects the airway node of the
 %Respiratory System to the mask/endotracheal node (referred as Connection 
in the circuit diagram) of the Mechanical Ventilator System. During spontaneous ventilation, the airway node of the
%Respiratory System is connected to the atmosphere via the %Environment System. 
This serves as a ground node for the %Respiratory System. 

When the Mechanical Ventilator is connected, a network of combined circuits that
include the elements from both the %Respiratory and Ventilator Systems is
created. When the combined circuit is generated at runtime, the
ground environment node connected to the mouth node of the %Respiratory System is
replaced by the Connection node that represents the mask/endotracheal node, 
becoming one combined circuit.
Apart from such interaction, the Ventilator System is
also responsive to the flow resistances of the %Respiratory System. In
this regard, the ventilator driver pressure serves as a positive-pressure source for 
the combined circuit. The Ventilator is linked to the %Environment 
System that regulates the atmospheric/reference pressure.

@anchor ventilator-assumptions
## Assumptions and Limitations

Currently, the Mechanical Ventilator uses ideal pressure sources and one-way valves. Only setting appropriate for a PC-CMV, VC-CMV, PC-AC, VC-AC, and CPAP modes are tested.  However, the system is defined and implemented to allow for future mode expansion without data model changes. 

@anchor ventilator-results
# Results and Conclusions

@anchor ventilator-breathing-simulator-validation
## Validation - Breathing Simulator Comparison

To validate functionality of our virtual ventilator model, we compared Pulse simulation results with physical ventilator values and waveforms. The RespiSim software allowed us to set the ASL 5000 equations of motion patient respiratory parameters (i.e., resistances, compliances, and spontaneous breath waveforms) to the same values as the Pulse respiratory system. The same ventilator settings were also applied to the physical Drager ventilator and the Pulse virtual ventilator model. We were able to create scenarios of spontaneously breathing patients with healthy lungs, restrictive disease, and obstructive disease in combination with PC-AC, VC-AC, and CPAP modes, for nine total validation use cases.

@htmlonly
<center><a href="./Images/MechanicalVentilator/PhysicalSetup.png"><img src="./Images/MechanicalVentilator/PhysicalSetup.png" style="width:40%;"></a></center>
@endhtmlonly
<center>
<i>Figure 3. The physical breathing simulator setup for virtual mechanical ventilator model validation is shown on the left.  The ASL 5000<sup>TM</sup> lung simulator (Model 31 00 150, SN:2226) was connected with a network cable to a laptop running RespiSim(R) software (version 4.0.10520). A Drager Evita(R) Infinity(R) V500 ventilator (SN:ASBD-0072) was connected with tubing to the ASL 5000. The patient respiratory physiological parameters and ventilator settings used for both the breathing simulator setup and the Pulse simulation are shown in the tables on the right.</i>
</center><br>

The waveforms created by the virtual ventilator match well with the displayed physical ventilator monitor values. The important temporal features and patterns used by Respiratory Therapists to manage patients @cite dexter2020ventilator are present. However, the data shows that an extended inspiration time by the physical ventilator translates to higher TVs than come from our ideal, deterministic circuit model. Discrepancies with the TV outputs for the PC-AC and CPAP modes can be attributed to imperfections with real-world sampling, lags from sensor triggering, non-instantaneous driver transitions, and differences in assumptions about air humidification in the patient's lungs. This follows expectations from previous lung model bench tests showed major differences between mechanical ventilator devices in both general characteristics and technical reliability @cite l2014bench. Most notable are large variability in TV delivery.

### Healthy Patient

<center>
<i>Table 1. The healthy patient respiratory physiological parameters used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Healthy-Patient-Settings.md

#### VC-AC Mode

<center>
<i>Table 2. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Healthy_VC_AC-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Healthy_VC_AC.jpg"><img src="./Images/MechanicalVentilator/Healthy_VC_AC.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/Healthy_VC_AC_Loops.jpg"><img src="./Images/MechanicalVentilator/Healthy_VC_AC_Loops.jpg"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_VC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 5. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 3. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Healthy_VC_AC-Validation.md

#### PC-AC Mode

<center>
<i>Table 4. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Healthy_PC_AC-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Healthy_PC_AC.jpg"><img src="./Images/MechanicalVentilator/Healthy_PC_AC.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/Healthy_PC_AC_Loops.jpg"><img src="./Images/MechanicalVentilator/Healthy_PC_AC_Loops.jpg"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_PC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 6. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 5. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Healthy_PC_AC-Validation.md

#### CPAP Mode

<center>
<i>Table 6. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Healthy_CPAP-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Healthy_CPAP.jpg"><img src="./Images/MechanicalVentilator/Healthy_CPAP.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/NoImage.png"><img src="./Images/MechanicalVentilator/NoImage.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Healthy_CPAP_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 7. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 7. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Healthy_CPAP-Validation.md

### Restrictive Patient

<center>
<i>Table 8. The restrictive patient respiratory physiological parameters used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Restrictive-Patient-Settings.md

#### VC-AC Mode

<center>
<i>Table 9. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Restrictive_VC_AC-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Restrictive_VC_AC.jpg"><img src="./Images/MechanicalVentilator/Restrictive_VC_AC.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/Restrictive_VC_AC_Loops.jpg"><img src="./Images/MechanicalVentilator/Restrictive_VC_AC_Loops.jpg"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_VC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 8. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 10. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Restrictive_VC_AC-Validation.md

#### PC-AC Mode

<center>
<i>Table 11. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Restrictive_PC_AC-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Restrictive_PC_AC.jpg"><img src="./Images/MechanicalVentilator/Restrictive_PC_AC.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/Restrictive_PC_AC_Loops.jpg"><img src="./Images/MechanicalVentilator/Restrictive_PC_AC_Loops.jpg"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_PC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 9. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 12. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Restrictive_PC_AC-Validation.md

#### CPAP Mode

<center>
<i>Table 13. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Restrictive_CPAP-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Restrictive_CPAP.jpg"><img src="./Images/MechanicalVentilator/Restrictive_CPAP.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/NoImage.png"><img src="./Images/MechanicalVentilator/NoImage.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Restrictive_CPAP_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 10. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 14. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Restrictive_CPAP-Validation.md

### Obstructive Patient

<center>
<i>Table 15. The obstructive patient respiratory physiological parameters used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Obstructive-Patient-Settings.md

#### VC-AC Mode

<center>
<i>Table 2. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Obstructive_VC_AC-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Obstructive_VC_AC.jpg"><img src="./Images/MechanicalVentilator/Obstructive_VC_AC.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/Obstructive_VC_AC_Loops.jpg"><img src="./Images/MechanicalVentilator/Obstructive_VC_AC_Loops.jpg"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_VC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 11. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 16. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Obstructive_VC_AC-Validation.md

#### PC-AC Mode

<center>
<i>Table 17. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Obstructive_PC_AC-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Obstructive_PC_AC.jpg"><img src="./Images/MechanicalVentilator/Obstructive_PC_AC.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/Obstructive_PC_AC_Loops.jpg"><img src="./Images/MechanicalVentilator/Obstructive_PC_AC_Loops.jpg"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_PC-AC_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 12. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 18. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Obstructive_PC_AC-Validation.md

#### CPAP Mode

<center>
<i>Table 19. The ventilator settings used for both the breathing simulator setup and the Pulse simulation.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Obstructive_CPAP-Ventilator-Settings.md

@htmlonly
<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 40%;">
    </colgroup>
    <tbody>
        <tr>
          <td rowspan="3"><a href="./Images/MechanicalVentilator/Obstructive_CPAP.jpg"><img src="./Images/MechanicalVentilator/Obstructive_CPAP.jpg"></a></td>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_AirwayPressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_AirwayPressure.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_InspiratoryFlow.png.jpg"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_InspiratoryFlow.png"></a></td>
        </tr>
        <tr>
          <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_Total_Lung_Volume.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_Total_Lung_Volume.png"></a></td>
        </tr>
    </tbody>
</table>

<table style="width: 100%">
    <colgroup>
       <col span="1" style="width: 60%;">
       <col span="1" style="width: 20%;">
       <col span="1" style="width: 20%;">
    </colgroup>
    <tbody>
        <tr>
        <td><a href="./Images/MechanicalVentilator/NoImage.png"><img src="./Images/MechanicalVentilator/NoImage.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_Total_Lung_Volume_vs_Airway_Pressure.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_Total_Lung_Volume_vs_Airway_Pressure.png"></a></td>
        <td><a href="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_Total_Lung_Volume_vs_Inspiratory_Flow.png"><img src="./plots/MechanicalVentilator/ventilator_analysis/Obstructive_CPAP_Total_Lung_Volume_vs_Inspiratory_Flow.png"></a></td>
        </tr>
    </tbody>
</table>
@endhtmlonly
<center>
<i>Figure 13. A comparison of waveforms and loops from the breathing simulator (left) and the Pulse simulation (right).</i>
</center><br>

<center>
<i>Table 20. A comparison of the breathing simulator ground truth values and the Pulse simulation results.</i>
</center><br>

@insert ./test_results/tables/MechanicalVentilator-Obstructive_CPAP-Validation.md


@anchor ventilator-validation-diseases
@insert ./validation/markdown/MechanicalVentilator/MechanicalVentilator.md


@anchor enroutecare
@insert ./validation/markdown/EnRouteCare/EnRouteCare.md


@anchor ventilator-conclusion
## Conclusion

While the model is a generic representation of mechanical ventilation, inhaled gases, and inhaled agent 
administration, this model represents the behavior of a complex piece of equipment that is 
associated with a difficult
medical specialty. The engine provides a whole-body approach to modeling that 
allows for simulation of this complex field. This system is a strong addition to the engine with the potential for 
future development.  This mechanical ventilator model
can be used in a standalone Pulse application or integrated with simulators, sensor interfaces, and models
of all fidelities. The data model has been carefully defined and implemented to allow for future expansion,
including the implementation of patient-specific physiological and pathophysiological models

@anchor ventilator-improvements
# Future Work

## Coming Soon

Logic and results for handling more ventilation modes.

## Recommended Improvements

The inherent modularity of this model could be leveraged to add parameters and elements for specific equipment models and manufacturers. 

@anchor ventilator-appendices
# Appendices

## Data Model Implementation

@ref MechanicalVentilatorTable "Mechanical Ventilator"

## Glossary

API - Application Programming Interface

ARDS - Acute %Respiratory Distress Syndrome

CDM - Common Data Model

CPAP - Continuous Positive Airway Pressure

FiO2 - Fraction of Inspired Oxygen

FRC - Functional Residual Capacity

I:E Ratio - Inspiratory-Expiratory Ratio

PaO2 - Arterial Oxygen Partial Pressure

PC-AC - Pressure Control - Assist Control

PC-CMV - Pressure Control - Continuous Mandatory Ventilation

PEEP - Positive End Expired Pressure

PIP - Peak Inspiratory Pressure

SpO2 - Oxygen Saturation

VC-AC - Volume Control - Assist Control

VC-CMV - Volume Control - Continuous Mandatory Ventilation
