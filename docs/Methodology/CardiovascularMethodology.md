%Cardiovascular Methodology {#CardiovascularMethodology}
==========================

@anchor cardiovascular-overview
Overview
========

Abstract
--------
The cardiovascular system is a closed-loop system of compartments that represents the human circulatory system. The cardiovascular system is sometimes described as two separate circulations: the systemic circulation and the pulmonary circulation. Both circulations are represented in the %Cardiovascular system, as well as the lymph. The dynamical state of the %Cardiovascular System is determined at every timestep through a three-step process. First, intra- and inter-system feedback is applied during the preprocess step. Next, the state of the circulatory system is determined by solving an equivalent circuit. Finally, the state of the system is updated in preparation of time advancement. Through system and action validation, we demonstrate the accuracy of the model at resting physiology and a variety of cardiovascular related insults and interventions with appropriate system feedback. 

Introduction
------------
### Cardiovascular Physiology
The cardiovascular system is a large organ system comprised of the heart and the blood vessels. It serves as the body's primary transport and distribution system. The cardiovascular system is sometimes described as two separate circulations: the systemic circulation and the pulmonary circulation. In the systemic circulation, oxygenated blood leaves the left side of the heart, travels  through arteries and into the capillaries, and then returns as deoxygenated blood through the veins to the right side of the heart. From the right side of the heart, the deoxygenated blood travels in the pulmonary circulation through the pulmonary arteries, is re-oxygenated in the pulmonary capillaries, and then returns to the left side of the heart through the pulmonary veins (@figureref {SystemDiagram}). Whether the cardiovascular circulation is thought of as one closed circuit with a single dual-purpose pump or two interconnected circuits with separate synchronous pumps, the key combination of systemic and pulmonary vasculature serves as a distribution and exchange network, providing vital oxygen to the tissues and removing toxic carbon dioxide while distributing nutrients and other substances necessary for healthy physiologic function.

\htmlonly
<center>
<a href="./Images/Cardiovascular/CardiovascularSystemDiagram.png">
  <img src="./Images/Cardiovascular/CardiovascularSystemDiagram.png">
</a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {SystemDiagram} The cardiovascular System with pulmonary and systemic circulations of the human body @cite KVDP2014Circulatory. 
Both the pulmonary and systemic circulation originate in the heart, which acts as a pump driving the blood through the entire body. The blood then returns to the right heart via the vena cava, where the right ventricle sends it to the pulmonary arteries and into the lungs. Gas exchange occurs in the vascular arterioles, creating oxygen-rich blood. This blood then returns via the pulmonary veins to the left atrium of the heart. The oxygen-rich blood enters the systemic circulation, providing oxygen to the rest of the body.</i>
</center>
<br>

The cardiovascular system is one of two systems that comprise the circulatory system, the other being the lymphatic system. The cardiovascular system includes a low-resolution lymphatic circulation. Because the resolution of the lymph circulation in the engine is considerably lower than the resolution of the cardiovascular circulation, we refer to the comprehensive circuit as the circulatory circuit, but we maintain the term *cardiovascular* as system nomenclature to highlight the modeling focus. In other words, the lymph circulation is not a physiological model of the human lymphatic system. It only serves as an accessory return for interstitial fluid. More information about the lymph model and the other accessory advection models is available in the @ref TissueMethodology report.

### Cardiovascular Modeling
#### Brief Introduction to Cardiovascular Modeling
%Cardiovascular (CV) models range from high-resolution, small-region-of-interest models @cite taylor1999predictive @cite wan2002one @cite formaggia2003one to low-resolution, whole-body simulations @cite heldt2004computational. Increased resolution may provide a more accurate solution in a localized region at the expense of decreasing the modeling region. However, the level of accuracy achieved with this model is not always required to answer the question posed. Three-dimensional (3D) computational fluid dynamics (CFD) models are generally limited to short segments of the arterial or venous tree due to high computational requirements. These high-resolution models are well-suited for investigating complex geometries following surgical intervention, such as the Fontan Circulation @cite whitehead2007nonlinear @cite de1996use. However, they are not suitable for modeling long-term or systemic effects in the CV System @cite timmons1995cardiovascular because it is only feasible to simulate a few cardiac cycles.

One-dimensional (1D) models can also predict the pressure and flow wave dynamics within a region of interest @cite womersley1955xxiv @cite womersley1957oscillatory and have the advantage of a lower computational cost than full CFD models. Like CFD models, 1D models begin with the Navier-Stokes equations, but they are reduced to a hyperbolic partial differential equation by including an axisymmetric assumption. The resultant equation dictates a constant pressure in the radial direction if the radius of the vessel is small relative to a characteristic wave length @cite vcanic2003mathematical. These models are well-suited for investigating specific CV problems @cite spilker2007morphometry @cite clipp2009impedance and have been coupled with dynamic downstream effects to investigate problems over numerous cardiac cycles @cite clipp2012evaluation. However, the computational cost associated with 1D models still renders them unsuitable for modeling the complete closed-circuit dynamics of the circulation over the entire range of vessel diameter and lengths in the human body.

The lumped parameter method of hemodynamic modeling assumes characteristics of organ vessels or vessel segments can be  &ldquo;lumped&rdquo; into representative parameters. For instance, the viscosity of a fluid can be lumped together with the geometry of a vessel segment to derive an effective resistance to blood flow. If a tube is rigid, the pressure-flow dynamics are completely characterized by the resistance to flow in the tube. If a tube is elastic, an additional compliance parameter is used to characterize the impedance and calculate the pressure-flow dynamics in the segment. In humans, small arteries can be approximately rigid @cite olufsen2004deriving, and the corresponding veins can be significantly more compliant (\~24 times more compliant) @cite hall2011guyton.

Otto Frank first formulated the quantitative model (called a Windkessel model after the wave-filtering air chamber in archaic firefighting equipment) relating systemic blood pressure with vessel elasticity @cite frank1899grundform. 
In the analogous electrical circuit (see @ref CircuitMethodology), the Windkessel model includes a capacitor, representing the compliance of the artery, connected in parallel with a resistor, representing the resistance to flow through the vessel. An alternate three-element form of the Windkessel model was developed by Westerhof et al. @cite westerhof1969analog that included a black-box, two-terminal input circuit, represented as an additional resistor, to quantify the characteristic impedance of the arterial tree. Similar applications of the electric analogue circuit have been applied to form CV circuits that can be classified as multi-segment or multi-system models.

#### Lumped Parameter Model
Lumped parameter models have proven suitable for larger-scale, longer time-scale simulations due to lower computational requirements and sufficient fidelity. A multi-organ model can run in real time while still producing an accurate blood pressure and flow waveform prediction. The drawback to this type of model is the large number of parameters requiring definition or tuning. However, significant work has been targeted at determining appropriate parameter values @cite bronzino1999biomedical @cite kokalari2013review @cite milivsic2004analysis @cite olufsen2004deriving @cite rideout1967difference @cite westerhof2009arterial. This body of work, in particular Heldt&rsquo;s open source CV simulator that includes lumped parameter models @cite heldt2004computational @cite hester2011hummod @cite summers2008development, supports the decision to implement these models in the engine.

@anchor cardiovascular-system-design
System Design
=============
Background and Scope
--------------------
### History of the Cardiovascular Model
The CV Model has its origins in Guyton&apos;s four-compartment (three vascular, plus one heart) model of the CV System designed to analyze the effect of varying circulatory factors on cardiac output @cite guyton1955effect. Rideout, et al. used Guyton&rsquo;s foundation and an electric-hydraulic analogy to streamline the generation of difference-differential equations for modeling fluid flow in distensible vessels @cite rideout1967difference. Yasuhiro Fukui leveraged the previous work to model the CV and respiratory systems and their interactions @cite fukui1972study. The development of a mass and momentum transport model of the CV System allowed for the simulation of interactions between the CV System and angiotensin @cite masuzawa1992cardiovascular. Following the success of the angiotensin-cardiovascular simulator, development of the drug-interaction model continued and eventually led to an anesthesiology simulator that incorporated CV, respiratory, and drug models @cite smith1995pc. This simulator, released by Advanced Simulation Corporation as Body Simulation for Anesthesia&tm;, formed the backbone of the HumanSim&tm; physiology engine, which continues to provide realistic physiology for several serious training games in the HumanSim product line, including HumanSim: Sedation and Airway @cite Clipp2012Humansim. The HumanSim physiology engine is the starting-point for the engine. The basic building blocks of the CV System remain as described in Masuzawa et al. @cite masuzawa1992cardiovascular; however, the CV circuit has been further developed to provide a more accurate CV simulation and drive other systems through intersystem dependencies.

Data Flow
---------
The state of the CV System is determined at every time step through a three-step process: Preprocess, Process, and Postprocess. In the Preprocess step, feedback from other systems, as well as intrasystem feedback, is processed in preparation for determining the state of the system. Process uses the [circuit](@ref CircuitMethodology) calculator to compute the new state of the system. Postprocess is used to prepare the system for the advancement of time. More specifics about these steps are detailed below.

@anchor cardiovascular-initialize
### Initialization and Stabilization
First, the patient-specific homeostatic state of the cardiovascular circuit is computed. Next, all system parameters are initialized. The %Cardiovascular System is then initialized to the resting state. Conditions are then applied by modifying system and patient parameters and restabilizing the engine. The available @ref cardiovascular-conditions "conditions" in the %Cardiovascular System are anemia, heart failure, pericardial effusion, and pulmonary shunt.

#### Tune Circuit
In the tune circuit step, the resistors and capacitors associated with tissue compartments are tuned during stabilization to achieve the mean arterial pressure given in the patient file.

### Preprocess
#### Begin Cardiac Cycle
Cardiac cycle calculations include methodology for updating the driving force (heart contraction and relaxation) of the CV System throughout the duration of a CV cycle (a single heart beat). This includes a set of systolic calculations that updates contractility at the beginning of the cycle to represent a heart contraction. Modifications to heart rate and heart compliance are calculated by BeginCardiacCycle and applied for the remainder of the current cardiac cycle. Changes to things like heart rate and heart contractility can only occur at the top of the current cardiac cycle after the last cardiac cycle has completed. This helps to avoid discontinuous behavior such as the complete cessation of heart function mid-contraction.

#### Calculate Heart Elastance
This method tracks the progress of the current cardiac cycle and modifies the compliance of the left and right heart to drive the cardiovascular circuit. The reduced compliance at the beginning of the cycle acts to increase the pressure, driving flow out of the heart. The compliance is then reduced, allowing flow into the heart. The reduction and increase in compliance represents the systolic and diastolic portions of the cardiac cycle, respectively. The compliance is driven by a Hill-type elastance equation @cite stergiopulos1996elastance.

#### Process Actions
Process Actions modifies the CV parameters and circuit properties due to actions (insults or interventions) specified by the user. The @ref cardiovascular-actions "actions" found in the action process are: CPR, hemorrhage, pericardial effusion, and cardiac arrest.

### Process
The generic circuit methodology developed for the engine is used to solve for the pressure, flow, and volume at each node or path in the equivalent circuit. For more details, see @ref CircuitMethodology.

#### Calculate Vital Signs
This function takes the current timestep&rsquo;s circuit quantities to calculate important system-level quantities for the current time step. The system pressures and flow rates related to shunting are calculated here. In addition, the events hypovolemia, tachycardia, bradycardia, and asystole are triggered in this function.

### Postprocess
The Postprocess step moves everything calculated in Process from the next time step calculation to the current time step calculation. This allows all other systems access to the information when completing their Preprocess analysis for the next time step.

### Assessments
Assessments are data collected and packaged to resemble a report or analysis that might be ordered by a physician. No assessments are associated with the %Cardiovascular system.

@anchor cardiovascular-features
Features, Capabilities, and Dependencies
----------------------------------------
### The Cardiovascular Circuit
The CV circuit (Figure 2) estimates blood pressure, flow, and volume for organs that are represented by several compartments. These compartments are comprised of lumped parameter models that use resistors and capacitors. Inductors may also be used to model inertial effects. The system is discretized into nodes that are connected by paths (see @ref CircuitMethodology). The circuit used to represent the CV System was designed to provide a level of resolution and fidelity that meets the [requirements](@ref MainPageFAQ) of the overall project.

For example, to provide a means for clearing drugs and substances from the bloodstream, the liver and kidneys must have blood flow, pressure, and volume calculations. Another example is the four extremities (right and left arms and legs) that provide extremity hemorrhage capabilities, having been implemented in a previous project (HumanSim: Combat Medic). In this way, the lumped parameter model provides a mechanism for increasing fidelity as required by the anatomic region or physiologic condition being modeled. The large thoracic arteries are lumped together into one &ldquo;Aorta&rdquo; compartment, which is represented by four nodes and three paths. The fidelity of any compartment could be easily improved by increasing the level of discretization. By adding nodes and paths, the engine &ldquo;Aorta&rdquo; could become the &ldquo;Ascending Aorta&rdquo; and &ldquo;Descending Aorta&rdquo; to accommodate the fidelity demands of the other systems. This could provide an opportunity to model more complex geometries and pathologies, such as stenosis. @figureref {CircuitDiagram} shows the cardiovascular circuit. For clarity, the more discretized [renal circuit](@ref renal-circuit) is not shown in this diagram. 

@htmlonly
<center>
<a href="./Images/Cardiovascular/CardiovascularCircuit.png"><img src="./Images/Cardiovascular/CardiovascularCircuit.png" width="900"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figureref {CircuitDiagram} The cardiovascular circuit consists of nodes that are connected via paths. These segments of nodes and paths are mapped to several compartments which represent the anatomy of the cardiovascular system. The circuit is used to estimate the blood pressure, flow, and volume of these anatomical compartments.</i>
</center><br>

Nodes serve as the connection points for paths and are the locations at which pressures are measured. Each CV node contains a pressure value, which is given with respect to the atmospheric reference node (indicated in the diagram by the equipotential symbol). Paths contain information about the flow (volume per time). The @ref CircuitMethodology document contains more information about circuit definitions and modeling. The @ref SubstanceTransportMethodology contains more information about the substance transport. In general, nodes contain "across" information and paths contain "through" information.

The elements of the CV circuit are used to model the fluid dynamics of the human CV System (hemodynamics). The hemodynamic pressure and flow are calculated from the lumped parameters that are determined by the circuit element. The equations used to calculate pressure and flow are shown below. These equations are automatically generated and solved simultaneously by the [Circuit Solver](@ref CircuitMethodology).

Derived values for the hemodynamic parameters are available, particularly for specific vessel segments @cite olufsen2004deriving @cite rideout1972cardiovascular. However, in the engine, these parameters are tuned to provide the physiologic response of a given [patient profile](@ref PatientMethodology). We completed the tuning process by choosing estimates for each parameter value based on the existing system values (Body and HumanSim) or based on physiologic data. We then analyzed the model outputs and adjusted parameters to obtain organ and system-level outputs that satisfied the validation requirements. The @ref cardiovascular-results "Results and Conclusions" section describes validation in more detail.

### The Heart
The heart model generates pressure that drives the hemodynamics through a variable capacitor that simulates the changing elastance of the myocardium throughout the cardiac cycle. The simulated heart has two sides, left and right, simulating the two sides of the human heart. The atria are not included in the heart model; only the ventricular behavior is modeled.

#### Heart Elastance and Compliance
The heart compliance is calculated from the inverse of the heart elastance. The heart elastance model used is adapted from the one developed by Stergiopulos et al @cite stergiopulos1996elastance. This model utilizes a double Hill function to represent heart elastance over the cardiac cycle time period. It was chosen due to its ability to scale with increasing or decreasing cardiac cycle times. The functional form for elastance of both left and right ventricles is shown in @equationref {elastance1} and @equationref {elastance2}.

\f[E_{v} (t)=(E_{\max ,v} -E_{\min ,v} )\left(\frac{f(t)}{f_{\max } } \right)+E_{\min ,v} \f]
<center>
*@equationdef {elastance1}*
</center><br>

Where *E<sub>max,v</sub>* is the maximum ventricle elastance in mmHg per mL. *E<sub>min,v</sub>* is the minimum ventricle elastance in mmHg per mL. *f(t)* is the double Hill function, and *f<sub>max</sub>* is the maximum value of the double Hill over the cardiac cycle length.

\f[f(t)=\left[\frac{\left(\frac{t}{\alpha _{1} T} \right)^{n_{1} } }{1+\left(\frac{t}{\alpha _{1} T} \right)^{n_{1} } } \right]\left[\frac{1}{1+\left(\frac{t}{\alpha _{2} T} \right)^{n_{2} } } \right] \f]
<center>
*@equationdef {elastance2}*
</center><br>

Where &alpha;<sub>1</sub> , &alpha;<sub>2</sub> , *n<sub>1</sub>*, and *n<sub>2</sub>* are shape parameters used to determine the distribution of the double Hill function. *T* is the cardiac cycle time period and *t* is the current time within the cardiac cycle.

The relationship between the elastance and compliance in the engine is shown in @equationref {ComplianceandElastance}.

@htmlonly
<center>
<a href="./plots/Cardiovascular/ComplianceVsElastance.jpg"><img src="./plots/Cardiovascular/ComplianceVsElastance.jpg" width="1100"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {ComplianceandElastance} The left heart compliance and elastance are shown to be inversely related to each other. The elastance represents the change in pressure per change in volume, while the compliance is the change in volume per change in pressure. These quantities define the contraction of the heart, which drives the pressure and flow of the cardiovascular circuit.</i>
</center><br>

#### Heart Pressure, Volume, and Flow
The variable compliance, which is used to model heart contraction and relaxation, yields pressure and volume changes that drive the flow through the CV circuit. This variable compliance driver allows the pressures and volumes to be calculated within the heart, as shown in @equationref {PressurevsVolume}.


@htmlonly
<center>
<a href="./plots/Cardiovascular/PressureVsVolume.jpg"><img src="./plots/Cardiovascular/PressureVsVolume.jpg" width="1100"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {PressurevsVolume} Relationship between pressure and volume in the left heart throughout the cardiac cycle. The relaxation of the heart muscle is modeled by increasing the compliance, resulting in an increase in left heart volume with a relatively constant left heart pressure. The contraction is represented by a rapid decrease in the compliance, leading to large pressure increases for small volume additions. This large pressure value drives the fluid out of the heart with flow rates calculated based on the circuit solution.</i>
</center><br>

A pressure-volume curve is used to represent the evolution of the cardiac cycle from the systolic contraction to diastolic relaxation. The pressure-volume curve for the left ventricle is shown in @equationref {PVLoop}. Starting from the bottom left and moving clockwise, the curve demonstrates a rapid increase in pressure with no change in volume. This indicates the systolic contraction of the cardiac cycle. Following this, the pressure declines rapidly as the heart expands during diastole. The last portion of the curve shows decreasing volume at constant pressure. Normally, the pressure would decrease slightly due to the imperfect mitral valve, which does not close instantly. The engine uses ideal valves, which close instantaneously, causing the pressure to be maintained as volume decreases.

@htmlonly
<center>
<a href="./plots/Cardiovascular/PVLoop.jpg"><img src="./plots/Cardiovascular/PVLoop.jpg" width="400"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {PVLoop} The pressure-volume curve for the left ventricle is represented as a pressure vs. volume plot. It demonstrates the the contracting and relaxing portions of the cardiac cycle. In addition, the curve demonstrates the use of ideal valves in the simulated heart due to instantaneous changes in volume at a set pressure.</i>
</center><br>

### Drug Effects
As drugs circulate through the system, they affect the %Cardiovascular System. The drug effects on heart rate, mean arterial pressure, and pulse pressure are calculated in the %Drugs System. These effects are applied to the heart driver by incrementing the frequency of the heart based on the system parameter calculated in the %Drugs System. Additionally, the mean arterial pressure is modified by incrementing the resistance of the blood vessels. At each timestep, the resistors are incremented on each cardiovascular path. In the future, the pulse pressure will be modified by changing the heart elastance. However, no drugs had pulse pressure changes; therefore, this step has not been necessary. The strength of these effects are calculated based on the plasma concentration and the drug parameters in the substance files.  For more information on this calculation, see @ref DrugsMethodology.

### Electrocardiogram
The engine electrocardiogram (%ECG) machine outputs an %ECG waveform to represent the currently supported heart rhythms.
The waveform is not a calculated value; the current %ECG system does not model the electrical activity of the heart.
This data is stored in a text file.
To account for the variable heart rate, rhythms are time series of voltage that is representative of a single cardiac cycle.
The points are then interpolated based on the length of the cardiac cycle.

@equationref {SinusECG} shows the lead 3 sinus waveform in Pulse compared to an example sinus waveform with the key features highlighted.

@htmlonly
<center>
<table>
<tr>
<td><a href"./Images/Cardiovascular/BasicECGWaveform.png"><img src="./Images/Cardiovascular/BasicECGWaveform.png" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/SinusECG.jpg"><img src="./plots/Cardiovascular/SinusECG.jpg" width="600"></a>
</td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {SinusECG} The ECG system produces a normal sinus waveform with the expected features.</i>
</center><br>

@anchor cardiovascular-variability
### Patient Variability
The cardiovascular system is heavily dependent on the patient. The cardiovascular circuit parameters are all set based on the patient configuration. For example, the proportion of blood flow to the fat compartment depends on the sex of the patient, and the initial flow proportions dictate initial vascular resistance. Likewise, the volume of the fat compartment depends on the sex, body fat fraction, height, and weight of the patient, and the initial volume and pressure are used to compute the initial vascular compliance for paths associated with a compartment. After initial values are computed, the cardiovascular circuit parameters are adjusted to meet target pressures, which are also specified in the patient file. Because the cardiovascular system is profoundly dependent on the patient configuration, several stabilization iterations are sometimes necessary to achieve a specific desired state. For that reason, we make modifications to the parameters to move the circuit closer to a Standard patient state to speed stabilization for that patient, and other custom states may require longer stabilization periods. A detailed discussion of patient configuration and variability is available in the @ref PatientMethodology report.
@anchor cardiovascular-dependencies
### Dependencies
The other engine systems depend on the CV System to function accurately and appropriately. The CV System provides the medium (blood) and the energy (pressure differentials and subsequent flow) to transport substances throughout the engine. A complete list of substances and their descriptions can be found in the @ref SubstanceTransportMethodology. Similarly, the CV System is dependent on all of the other systems. Feedback from other systems influences the cardiac behavior via heart rate and elastance modifiers and the systemic circulation through resistance modifications to the circuits. For example, as the plasma concentration of a drug increases, the elastance of the heart, heart rate, and systemic resistance are modified through the pharmacodynamic effects (multipliers), resulting in changes to the blood flow, pressure, and volume calculations. Additionally, baroreceptor feedback leads to modification of heart rate, elastance, and vascular resistance and compliance in order to regulate the arterial pressure. For more information on baroreceptor feedback see @ref NervousMethodology.

Gas exchange is one area that closely ties the respiratory system, substances, and the CV system together. Gas exchange is calculated based on the partial pressures of the gases in the lungs and the capillary beds (@ref SubstanceTransportMethodology). If respiration was to stop due to a drug effect or physiologic condition, such as airway obstruction, the gas exchange calculations would be affected. Oxygen would stop diffusing into the CV System from the %Respiratory System, resulting in an oxygen deficit in the cardiac tissue.

@anchor cardiovascular-assumptions
Assumptions and Limitations
---------------------------
The constant-compliance equations in the CV System assume a linear diastolic pressure-volume relationship in the vessels (&Delta;V = C&Delta;P) representative of an elastic vessel wall. This assumption is appropriate for many blood vessels under normal physiologic conditions; however, research shows that many vessel walls are more appropriately modeled as visco-elastic walls @cite alastruey2011visco. While the non-linear pressure-volume relationship within the heart is modeled using a variable capacitor, the visco-elastic nature of the blood vessels is not addressed in this model. Rigid and elastic wall properties have been used to model the hemodynamic behavior in high-fidelity models with success @cite alastruey2011visco. Therefore, the level of fidelity of the lumped parameter model and resulting simulation support the use of elastic walls.

The CV model has a waveform limitation because of our ideal diode model. We assume the valves are perfect Booleans. In reality, there is a transition time between the open and closed states of a valve. During this time, a small amount of retrograde flow may be present @cite halliwill2010retro . In the future, we hope to incorporate retrograde flow through a Zener diode. However, the current methodology does allow retrograde flow at any location without a diode. The Zener diode implementation will provide a mechanism for future modeling efforts, including abnormal heart valves that result in significant retrograde flow.

@anchor cardiovascular-conditions
Conditions
----------
#### Anemia
Anemia conditions reduce the oxygen-carrying capacity of the blood. The engine models iron deficiency anemia as a chronic condition, which is characterized by a decrease in hemoglobin concentration and subsequent decreases in hematocrit and blood viscosity @cite Duke1969Hemodynamic. These factors lead to a decrease in systemic vascular resistance @cite guyton2006medical. The engine currently supports up to a 30% decrease in hemoglobin. After engine stabilization, the chronic condition reduces the hemoglobin throughout the circuit and reduces the systemic vascular resistance to represent the change in viscosity. The engine then re-stabilizes based on the chronic condition criteria. For more information, see @ref SystemMethodology. There is an observable increase in venous return due to the decreased systemic vascular resistance. As validation data supports, there are no observable effects from the decreased oxygen-carrying capacity at rest. These effects will be evident in the future with incorporation of exercise. This condition is currently not validated.

#### Heart Failure
Heart failure refers to a malfunctioning of the heart, resulting in inadequate cardiac output. The current mode of heart failure being modeled in the engine is chronic left ventricular systolic dysfunction. After engine stabilization, the chronic heart failure is modeled with a 45% reduction in the contractility of the left heart. After the engine has re-stabilized using the chronic condition criteria (@ref SystemMethodology), the heart ejection fraction of 60% is reduced to 31% as noted in the validation data @cite chatterjee2007systolic. The reduced contractility leads to a decrease in heart stroke volume, which causes an immediate drop in the cardiac output and arterial pressures. The baroreceptor reflex responds with an increase in heart rate in an attempt to return mean arterial pressure to its normal value. This model is currently not validated.

#### Pericardial Effusion
A chronic pericardial effusion is an accumulation of fluid in the pericardial sac that surrounds the heart. This accumulation occurs over a long period of time, resulting in the body making some accommodation to these changes. The result is an increased pressure on the heart, which leads to strained performance. This performance strain is less severe than in an acute effusion. Due to this increasing pressure, there is a decrease in stroke volume, which leads to a decrease in cardiac output and arterial pressures. To compensate for reduced stroke volume, the baroreceptor response raises the heart rate in an attempt to restore cardiac output.

#### Pulmonary Shunt
The pulmonary shunt generally models an unspecified change in the amount of blood that is shunted away from alveolar gas exchange in the pulmonary capillaries. This will impair the gas diffusion that occurs between the cardiovascular and respiratory systems. The pulmonary shunt can be specified on either the right, left, or both pulmonary vasculature and specified with a severity between 0 and 1.


@anchor cardiovascular-actions
Actions
-------
### Insults

#### Arrhythmia
An arrhythmia is a deviation from normal cardiac heart rate and function.

Currently, we support %ECG data for the following arrhythmias

 - Asystole
 - Sinus Tachycardia
 - Sinus Bradycardia
 - Pulseless Electrical Activity (PEA)
 - Coarse and Fine Ventricular Fibrillation
 - Stable and Unstable Ventricular Tachycardia
 - Pulseless Ventricular Tachycardia

We have an arrhythmia action with the type of arrhythmia as the input.
All of the arrhythmias were implemented as types that can be specified as part of the arrhythmia action.
In general, it takes about 60s to trasition between arrhythmia's.

<b>Asystole</b>

The transition to asystole is instantaneous.
We set the heart rate to 0 to reflect a lack of cardiac function @cite ACLS2021asystole.
All feedback and imapcts from additional actions will NOT impact the hemodynamics of the cardiovascular system, as there is no hemodynamic activity during this arrhythmia.

<center>
@htmlonly
<a href="./plots/Cardiovascular/Asystole_Engine.jpg"><img src="./plots/Cardiovascular/Asystole_Engine.jpg" width="250"></a>
<br>
@endhtmlonly
<i>@figuredef {ECGAsystole} The ECG waveform is set to 0 volts to represent the lack of electrical activity @cite ACLS2021asystole in asystole.</i>
</center><br>

<b>Sinus Tachycardia</b>

The heart rate baseline is set to 130.
Pulse will transition the patient to this heart rate over 60s.
The ECG will use the shape of the normal sinus rhythm as its basis.
As the heart rate baseline is modified, the output will update at the new heart rate frequency.
All feedback and imapcts from additional actions will still impact the hemodynamics of the cardiovascular system from this new starting rate.

<center>
@htmlonly
<table>
<tr>
<td><a href="./Images/Cardiovascular/SinusTachycardia_ACLS.jpg"><img src="./Images/Cardiovascular/SinusTachycardia_ACLS.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/SinusTachycardia_Engine.jpg"><img src="./plots/Cardiovascular/SinusTachycardia_Engine.jpg" width="550"></a>
</td>
</tr>
</table>
<br>
@endhtmlonly
<i>@figuredef {ECFSinusTachycardia} Due to the high heart rate, the engine output is summing together the P and T waves. In the image from PhysioNet, the output is not summed together as dramatically, due to the slight physiological compression of the waveform that the current %ECG system and heart model do not support. @cite healey2005detecting @cite goldberger2000physiobank</i>
</center><br>

<b>Sinus Bradycardia</b>

The heart rate baseline is set to 50.
Pulse will transition the patient to this heart rate over 60s.
The ECG will use the shape of the normal sinus rhythm as its basis.
As the heart rate baseline is modified, the output will update at the new heart rate frequency.
All feedback and imapcts from additional actions will still impact the hemodynamics of the cardiovascular system from this new starting rate.

<center>
@htmlonly
<table>
<tr>
<td><a href="./Images/Cardiovascular/SinusBradycardia_ACLS.jpg"><img src="./Images/Cardiovascular/SinusBradycardia_ACLS.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/SinusBradycardia_Engine.jpg"><img src="./plots/Cardiovascular/SinusBradycardia_Engine.jpg" width="550"></a>
</td>
</tr>
</table>
@endhtmlonly
<br>
<i>@figuredef {SinusBradycardia} The increased R-R interval is evident in both waveforms. This is the primary indication of the low heart rate. Validation image courtesy of @cite vanderBilt2010sinus .</i>
</center><br>

<b>Pulseless Electrical Activity (PEA)</b>

Sinus pulseless electrical activity (PEA) is characterized by organized electrical activity of the heart, but no mechanical function @cite ACLS2021Pulseless.
For sinus PEA, the ECG is set to a normal sinus rhythm with a heart rate of 65 bpm; however, the amplitude of the waveform is reduced to reflect the weaker electrical activity @cite ACLS2021Pulseless.
The transition to PEA is instantaneous.
All feedback and imapcts from additional actions will NOT impact the hemodynamics of the cardiovascular system, as there is no hemodynamic activity during this arrhythmia.

<center>
@htmlonly
<a href="./plots/Cardiovascular/SinusPEA_Engine.jpg"><img src="./plots/Cardiovascular/SinusPEA_Engine.jpg" width="500"></a>
<br>
@endhtmlonly
<i>@figuredef {ECGPEA} PEA is characterized by organized electrical activity in a normal sinus rhythm shape with a reduced amplitude @cite ACLS2021Pulseless.</i>
</center><br>

<b>Ventricular Fibrillation Arrhythmias</b>

Ventricular fibrillation is characterized by a lack of mechanical function of the heart and disorganized electrical activity @cite ACLS2021Pulseless.
As no cardiac cycle is associated with ventricular fibrillation, the ECG uses a representative period of disorganization (several seconds) was extracted from an arrhythmia database @cite PhysioNetDatabaseCUVentricular.
This representative dataset is then repeated throughout the duration of this arrhythmia.
The transition to ventricular fibrillation is instantaneous.

Ventricular fibrillation is defined as either coarse or fine.
Coarse ventricular fibrillation is represented by more electrical activity than fine ventricular fibrillation.
The amplitude is reduced to half for fine ventricular fibrillation rhythm.
All feedback and imapcts from additional actions will NOT impact the hemodynamics of the cardiovascular system, as there is no hemodynamic activity during this arrhythmia.

<center>
@htmlonly
<table>
<tr>
<td colspan="2">
<a href="./Images/Cardiovascular/VentricularFibrillation_ACLS.jpg"><img src="./Images/Cardiovascular/VentricularFibrillation_ACLS.jpg"></a>
</td>
</tr>
<tr>
<td><a href="./plots/Cardiovascular/ECGCoarseVF_Engine.jpg"><img src="./plots/Cardiovascular/ECGCoarseVF_Engine.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/ECGFineVF_Engine.jpg"><img src="./plots/Cardiovascular/ECGFineVF_Engine.jpg" width="550"></a>
</td>
</tr>
</table>
<br>
@endhtmlonly
<i>@figuredef {ECGVentricularFib} Ventricular fibrillation is characterized by disorganized electrical activity. Coarse (Left) has higher electrical signal than fine (Right) ventricular fibrillation @cite ClevelandClinic2021vfib. </i>
</center><br>

<b>Ventricular Tachycardia Arrhythmias</b>

Ventricular tachycardia is defined as either stable or unstable.
Stable ventricular tachycardia is characterized by increased heart rate (between 100 and 150) with little change in blood pressure (hemodynamic stability) and an irregular ECG @cite ACLS2021Tachy @cite LearningNetwork2021stable.
Unstable ventricular tachycardia is characterized by a heart rate of over 150 @cite ACLS2021Tachy with a significant drop in blood pressure (hemodynamic instability) @cite Wegria1958effect and the ventricular tachycardic ECG waveform.

<center>
@htmlonly
<table>
<tr>
<td><a href="./Images/Cardiovascular/VentricularTachycardia_Exp.png"><img src="./Images/Cardiovascular/VentricularTachycardia_Exp.png" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/ECGVT_Engine.jpg"><img src="./plots/Cardiovascular/ECGVT_Engine.jpg" width="550"></a>
</td>
</tr>
</table>
<br>
@endhtmlonly
<i>@figuredef {ECFVentricularTachy} This ventricular tachycardia ECG waveform is used for both stable and unstable types and is scaled to the heart rate. </i>
</center><br>

Pulse will transition the patient to this heart rate over 60s.
For stable ventricular tachycardia, the heart rate baseline is set to 130.
For unstable ventricular tachycardia, the heart rate baseline is set to 160.
The blood pressure was reduced through for unstable ventricular tachycardia by adding systemic compliance and resistance modifiers.
The heart rate and blood pressure for stable and unstable ventricular tachycardia are shown in @equationref {ECGHRVentricularTachy}.
All feedback and imapcts from additional actions will still impact the hemodynamics of the cardiovascular system from this new starting rate.

<center>
@htmlonly
<table>
<tr>
<td><a href="./plots/Cardiovascular/HeartRateStableVentricularTachycardia.jpg"><img src="./plots/Cardiovascular/HeartRateStableVentricularTachycardia.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/HeartRateUnstableVentricularTachycardia.jpg"><img src="./plots/Cardiovascular/HeartRateUnstableVentricularTachycardia.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/BloodPressureStableVentricularTachycardia.jpg"><img src="./plots/Cardiovascular/BloodPressureStableVentricularTachycardia.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/BloodPressureUnstableVentricularTachycardia.jpg"><img src="./plots/Cardiovascular/BloodPressureUnstableVentricularTachycardia.jpg" width="550"></a>
</td>
</tr>
</table>
<br>
@endhtmlonly
<i>@figuredef {ECGHRVentricularTachy} Heart rate for stable (Far Left) and unstable ventricular (Middle Left) tachycardia meets the validation of 100-150 and greater than 150, respectively @cite ACLS2021Tachy.
Stable ventricular (Middle Right) tachycardia shows hemodynamic stability, while hemodynamic instability is present in unstable ventricular tachycardia (Far Right) @cite LearningNetwork2021stable @cite Wegria1958effect.</i>
</center><br>

Pulseless ventricular tachycardia are characterized by organized electrical activity of the heart, but no mechanical function @cite ACLS2021Pulseless.
The ECG is set to a ventricular tachycardia rhythm for pulseless ventricular tacycardia; however, the amplitude of the waveform is reduced to reflect the weaker electrical activity @cite ACLS2021Pulseless.
All feedback and imapcts from additional actions will NOT impact the hemodynamics of the cardiovascular system, as there is no hemodynamic activity during this arrhythmia.

#### Hemorrhage
@anchor hemorrhage-action
@insert ./validation/markdown/Hemorrhage_Action.md

#### Pericardial Effusion
The pericardial effusion action is used to model acute pericardial effusion by adding a flow source on the pericardium. This action leads to a volume accumulation over the course of the simulation. The accumulated volume is used to calculate a pressure source that is applied to the left and right heart. This pressure source is identical to the one used in the pericardial effusion condition. For the pericardial effusion action, the strain-rate dependent compliance of the pericardium is modeled so that the change in intrapericardial pressure is a function of flow rate and the current volume of the pericardium @cite Metoyer2014Modeling.

### Interventions

#### Cardiopulmonary Resuscitation (CPR)
CPR can be preformed to maintain some perfusion through an induced arrhythmia action. The American Heart Association recommends performing chest compressions at a rate of 100-120 per minute with enough force to achieve a chest-compression depth of 5 cm @cite berg2010adult. Gruben et al estimated a required force of approximately 400 N to achieve the required depth @cite gruben1990system. Kim et al found that stroke volumes of 12-35 mL and an effective cardiac output of 1200-3500 mL/min could be achieved with the proper compression technique. The aforementioned hemodynamics are accompanied by an increase in systolic and diastolic pressures and, by extension, mean arterial pressure @cite kim2008direction.

Chest compressions are simulated in the engine by adjusting the value of the pressure source that is connected between the common equipotential node and the capacitors that represent the right and left cardiac compliances. The pressure source represents the intrathoracic pressure, thus a positive pressure value represents an increase in intrathoracic pressure. The pressure on the source is calculated from the input force and the biometrics of the patient.

The CPR force of compression can be expressed to the engine in one of two ways, as a Force (ex. N), or depth between 0 and 6 cm, where 6m provides the maximum pressure of 550 N based on the work of Arbogast et al @cite arbogast2006anterior. Pulse provides several actions to prescribe chest compressions:

 - An instantaneous action: This action allows a user to explicitly set the applied force at the time of the CPR compression call. The provided pressure is maintained in the engine until the next action is provided. Successive compressions are achieved by calling a CPR compression at a desired force, calling for an advance in simulation time, and then calling the next desired force. In this way, the shape of the time-dependent force curve is explicitly set by the user. This action is primarily intended to be used in conjuction with hardware sensors providing Pulse continuous force information.
 - A single compression action: This action allows users to supply the force to be applied over a specified time. When the compression is applied in Pulse, the evolution of the force is a curve applied over the provided compression time automatically by the engine. A bell shaped force curve is used in accordance with @cite stanley2012recreating. Once the compression is complete, you must provide another action to keep performing CPR. Users will need to advance time without a compression action to simulate the wait time between compressions.
 - An automatic compression action: This action allows a user to prescribe a compression frequency to be continuously applied. This will continuously applying compressions at a fixed interval unitl a force or frequency of 0 is provided. A compression duration is derived from the provide frequency. To specify a wait time between compressions, the AppliedForceFraction parameter will inform the engine how much of the derived compression duration to apply the force curve. The remaining time of the compression will have 0 force. For example, a frequecy of 120 bpm will result in a compression duration of 0.5s. An AppliedForceFraction of 0.8, will result in the force curve being applied over 0.4s and 0 force being applie for 0.1s, then the next compression will begin.

The compression-only CPR in the engine is consistent with AHA guidelines for lay rescuer adult CPR @cite hazinski2010. The addition of rescue breathing for health-care provider CPR can be provided using a Bag Valve Mask action.

#### Hemorrhage Cessation
A &ldquo;tourniquet&rdquo; may be applied to a bleeding distal portion of the body to reduce blood flow entering that portion and effectively stopping the bleeding. With the bleeding managed, vital signs should return to their normal state after a sufficient period of time. A &ldquo;tourniquet&rdquo; may be simulated in the engine by reducing the bleeding rate associated with a specified hemorrhage. If the hemorrhage leads to significant blood loss, reducing the bleeding rate of that hemorrhage will lead to a return of the arterial pressures as the effect of the baroreceptor response is not outweighed by blood loss. As the MAP increases, heart rate will begin to decrease in proportion to the error control dictated by the baroreceptor reflex. Lastly, cardiac output will begin to increase to a new, stabilized value associated with the new total blood volume.

#### Intravenous Fluid Administration
Intravenous fluid administration is a continual injection of a compatible fluid into the veins of a patient that has suffered from fluid loss. Due to increasing volume from intravenous fluid administration, blood volume and arterial pressures will increase. Stroke volume will increase due to increased venous return, which will cause an increase in cardiac output. The baroreceptor response will lead to a decrease in the heart rate.

Intravenous fluid administration is simulated by applying a flow source to the vena cava. The flow source and duration of the administration is dictated by the user in the form of a flow rate and IV bag volume. This results in an increase in total blood volume, heart stroke volume, cardiac output, and arterial pressures. Due to increasing arterial pressures, the baroreceptor reflex will begin to decrease heart rate according to the error between the mean arterial pressure and its set point. Additional effects may occur depending on the type of fluid administered. Currently, the user may administer blood, saline, and/or packed red blood cells. Each of these fluids is considered a compound substance with multiple substance components. Further information on the effect of substances during fluid administration can be found in @ref SubstanceTransportMethodology.

#### Exacerbations

The pulmonary shunt condition has an exacerbation actions defined to allow for increased/decreased severities during runtime.  This exacerbation action will instantaneously (i.e., during the simulation/scenario runtime) change the value of the pulmonary shunt (right, left, or both), based on the severity provided.  Exacerbations can either degrade or improve the patient's current condition.

@anchor cardiovascular-events
##Events

@anchor cardiovascular-arrest
### Cardiac Arrest
Cardiac Arrest is a condition in which the pumping of the heart is no longer effective @cite nhlbi2011sca. The Cardiac Arrest event can be triggered by different arrhythmias in the engine. In the current version of the engine, any rhythm associated with a cessation of mechanical function of the heart will trigger a cardiac arrest event. We do not have a defibrillate action at this time. To recover from a cardiac arrest event, the user can change the arrhythmia back to a sinus rhythm. It is also possible to maintain some perfusion by performing chest compressions (see CPR).

### Cardiovascular Collapse
%Cardiovascular collapse occurs when the blood pressure is no longer sufficient to maintain "open" blood vessels. They "collapse" meaning blood can no longer flow through the vessels. This is generally associated with shock, the vascular tone and blood pressure are no longer sufficient to maintain blood flow @cite guyton2006medical. This occurs at at mean arterial pressure of 20 mmHg or lower in the engine. At this time, we do not enforce an irreversible state (stop engine calculation); however, the patient generally does not recover from this, particularly in the presence of shock.

### Cardiogenic Shock
In general, the term "shock" refers to inadequate perfusion of the tissues. The several categories of shock serve to signify the origin of the disturbance. Cardiogenic shock is inadequate perfusion due a reduction in the pumping capability of the heart. In the engine, the Cardiogenic Shock event is activated when the cardiac index () is below 2.2 (L/min-m^2) *and* the systolic blood pressure is less than 90.0 (mmHg) *and* the pulmonary capillary wedge pressure is greater than 15.0 (mmHg) @cite dhakam2008review.

### Hypovolemic Shock
Hypovolemia is any reduction in blood volume. Hypovolemic shock is defined as a reduction in total blood volume by 35 percent @cite guyton2006medical. Typically, this is classified by elevated heart rate and decreased arterial pressure. In the engine, hypovolemia is triggered during a hemorrhage when blood volume has fallen below 65 percent of its normal value. 

@anchor cardiovascular-results
Results and Conclusions
=======================
The %Cardiovascular System was validated quantitatively and qualitatively under resting physiologic conditions and transient (action-induced) conditions. The validation is specified with a color coding system, with green indicating good agreement with trends/values, yellow indicating moderate agreement with trends/values, and shades red indicating poor agreement with trends/values. 

@anchor cardiovascular-validation-resting
Validation - Resting Physiologic State
--------------------------------------
Validation results for system and compartment quantities are listed in @tableref {SystemValidation} and @tableref {CompartmentValidation}. System-level quantities show favorable agreement with validation values. Heart rate, arterial pressures, blood volume, heart stroke volume, and cardiac output are the predominant CV System quantities. These values agree, on average, within ~8 percent of the expected values for the healthy standard patient. 

<br><center>
*@tabledef {SystemValidation} Validation of the resting physiologic state comparison of system-level outputs from the engine to referenced values. System-level outputs show favorable agreement with validation data.*
</center>

<b>Standard Male</b>

@insert ./test_results/tables/Cardiovascular-StandardMaleValidationTable.md

<b>Standard Female</b>

@insert ./test_results/tables/Cardiovascular-StandardFemaleValidationTable.md

<br><center>
*@tabledef {CompartmentValidation} Validation of the resting physiologic state comparison of compartment-level outputs from the engine to referenced values. The compartments are currently validated on a flow/volume basis. Flows and most of the volumes show good agreement with validation values.*
</center>

<b>Standard Male</b>

@insert ./test_results/tables/CardiovascularCompartments-StandardMaleValidationTable.md

<b>Standard Female</b>

@insert ./test_results/tables/CardiovascularCompartments-StandardFemaleValidationTable.md

Compartment-level quantities show reasonable agreement with the validation values. All of the flows match the reference values within ~10 percent. The volumes show some moderate differences for a few specific compartments. The aorta compartment volume is much smaller than the validated value. The compliance on this compartment needed to remain low in order to preserve the arterial pressure waveform, which led to less volume than expected. Similarly, the vena cava compliance was set in order to maintain the correct cardiac output and arterial pressures; therefore, its expected volume was limited. The right heart pressures and volumes show some disagreement with the validation data. The minimum values for right heart pressure and volume are much lower than valid ranges. This is due to restriction of unstressed volume in the right heart, which currently has an unstressed volume of zero. An increase in unstressed volume would shift the pressure volume minimums up, while also preserving the maximum values within their respective ranges. The %Cardiovascular System is tuned to vitals output validation (Table 1), as well as good agreement with insults' and interventions&rsquo; expected trends and values (see the following section).  In addition, compartment validation was achieved on a reasonable level.

The arterial pressure waveform was validated according to the plot shown in Figure 13. It displays the engine arterial pressure against measured arterial pressure. The diastolic and systolic pressures were validated using data shown in Table 1. To validate the waveform shape and demonstrate the overall feature match of the engine pressure waveform with the  validation data, we used a waveform from PhysioNet @cite goldberger2000physiobank . However, the patient heart rate and parameters are slightly different than the engine patient. This led to timing discrepancies and differences in the diastolic and systolic pressures. To demonstrate the waveform feature matching, a separate axis is used for each data set. In the future, we may create a patient more representative of the one used for the PhysioNet waveform. This would show both the ability of the engine to model different patients and the pressure waveform feature matching.  The shapes of both waveforms match well, showing rapid pressure increases as the heart contraction begins to occur. The main difference in the shape of each plot is the small pressure oscillations that occur after the initial pressure drop in the validation data. This is the dicrotic notch,  which occurs from slight flow reversal from the aorta back into the left ventricle before the valves close @cite korakianitis2005notch . This is not currently being modeled in the engine, but improvements in the circuit model, including the addition of inertance and diodes that allow retrograde flow, will likely enable the engine waveform to capture more detail.

@htmlonly
<center>
<table>
<tr>
<td><a href="./plots/Cardiovascular/EnginePressure.jpg"><img src="./plots/Cardiovascular/EnginePressure.jpg" width="400"></a></td>
<td><a href="./plots/Cardiovascular/PhysioNetPressure.jpg"><img src="./plots/Cardiovascular/PhysioNetPressure.jpg" width="400"></a></td>
</tr>
</table>
</br>
</center>
@endhtmlonly
<center>
<i>@figuredef {ArterialPRessureWaveforms} Arterial pressure waveform comparisons. The diastolic and systolic pressures were validated using the data shown in Table 1. To validate the waveform shape and demonstrate the overall feature match of the engine pressure waveform with the  validation data, a waveform was found on PhysioNet @cite goldberger2000physiobank . However, the patient heart rate and parameters are slightly different than the engine patient. This led to timing discrepancies and differences in the diastolic and systolic pressures. To demonstrate the waveform feature matching, a separate axis is used for each data set. Both the validation waveform and the engine waveform show sharp increases in pressure during the systolic period. After the contraction occurs, the pressure begins decreasing and that is where the main difference in the engine and the validation data occur. There is a dip and subsequent rise in the arterial pressure that occurs due to the dicrotic notch, which the engine does not capture.</i>
</center><br>

@anchor cardiovascular-validation-conditions
Validation - Actions and Conditions
--------------------
All actions in the CV System were validated. A summary of this validation is shown in @tableref {ValidationSummary}. More details on each individual scenario's validation can be found below.

<center>
*@tabledef {ValidationSummary} Cumulative validation results for %Cardiovascular specific conditions and actions scenarios.*
</center>

|	Key	|
|	---	|
|<span class="success">	Good agreement: correct trends or <10% deviation from expected	</span>|
|<span class="warning"> 	Some deviation: correct trend and/or <30% deviation from expected	</span>|
|<span class="danger">	Poor agreement: incorrect trends or >30% deviation from expected	</span>|

|	Scenario 	|	Description	|	Good	|	Decent	|	Bad	|
|	---	|	---	|	---	|	---	|	---	|
|	Arrhythmia - Asystole To NormalSinus	|	Asystole for 3 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - NormalSinus To SinusBradycardia	|	Bradycardia for 2 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - Asystole To SinusBradycardia	|	Asystole for 2 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - NormalSinus To SinusTachycardia	|	Tachycardia for 2 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - Asystole To SinusTachycardia	|	Asystole for 2 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - Sinus Pulseless Electrical Activity To NormalSinus	|	PEA for 3 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - Coarse Ventricular Fibrillation To NormalSinus	|	Coarse VFIB for 3 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - Fine Ventricular Fibrillation To NormalSinus	|	Fine VFIB for 3 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - NormalSinus To Stable Ventricular Tachycardia	|	Stable VTACH for 2 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - Asystole To Stable Ventricular Tachycardia	|	Asystole for 2 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Arrhythmia - NormalSinus To Unstable Ventricular Tachycardia	|	Unstable VTACH for 2 min	|<span class="success">	6	</span>|<span class="warning">	0	</span>|<span class="danger">	1	</span>|
|	Arrhythmia - Asystole To Unstable Ventricular Tachycardia	|	Asystole for 2 min	|<span class="success">	6	</span>|<span class="warning">	0	</span>|<span class="danger">	1	</span>|
|	Arrhythmia - Pulseless Ventricular Tachycardia To NormalSinus	|	Pulseless VTACH for 3 min	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	CPR	|	Cardiac arrest is initiated, and CPR is performed.	|<span class="success">	22	</span>|<span class="warning">	2	</span>|<span class="danger">	0	</span>|
|	Hemorrhage Class 1 - Femoral	|	15% hemorrhage from femoral artery	|<span class="success">	13	</span>|<span class="warning">	1	</span>|<span class="danger">	0	</span>|
|	Hemorrhage Class 2 - Brachial	|	25% hemorrhage from right arm	|<span class="success">	12	</span>|<span class="warning">	2	</span>|<span class="danger">	0	</span>|
|	Hemorrhage Class 2 - Blood	|	25% hemorrhage then intravenous whole blood administration	|<span class="success">	14	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Hemorrhage Class 2 - Spleen	|	25% hemorrhage from spleen - internal hemorrhage	|<span class="success">	14	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Hemorrhage Class 2 - Saline	|	25% hemorrhage then intravenous saline administration	|<span class="success">	21	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Hemorrhage Class 3 - No Fluids	|	35% hemorrhage	|<span class="success">	13	</span>|<span class="warning">	0	</span>|<span class="danger">	1	</span>|
|	Hemorrhage Class 3 - RBC	|	30% hemorrhage then packed red blood cell administration	|<span class="success">	19	</span>|<span class="warning">	1	</span>|<span class="danger">	1	</span>|
|	Hemorrhage to Shock	|	Hemorrhage until death	|<span class="success">	13	</span>|<span class="warning">	0	</span>|<span class="danger">	1	</span>|
|	Pericardial Effusion - Chronic	|	Patient has an effused pericardium with an accumulated volume of 500 ml.	|<span class="success">	9	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Pericardial Effusion - Acute	|	Pericardium volume starts at 500mL and increases at 6 mL/min.	|<span class="success">	17	</span>|<span class="warning">	1	</span>|<span class="danger">	0	</span>|
|		|	Total	|<span class="success">	256	</span>|<span class="warning">	7	</span>|<span class="danger">	5	</span>|

### Arrhythmia
There are 13 scenarios for arrhythmia validation.
Each arrhythmia is validated in its own scenario.
Each scenario starts with a stabilized patient in normal sinus, enter an arrhythmia for several minutes, then returns to normal sinus.
Several additional scenarios test going to asystole and returning to each of the active (non pulseless) cardiac rhythms.

#### Asystole
<br><center>
*@tabledef {AsysoleValidation} Validation matrix for going from normal sinus, to asystole, back to normal sinus. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	Asystole To NormalSinus	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Asystole	|		|	30	|	180	|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	Flatline/Asystole @cite ACLS2021asystole	</span>|
|	Normal Sinus	|		|	210	|	300	|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|


#### Sinus Bradycardia

<br><center>
*@tabledef {SinusBradycardiaVal} Validation matrix for going from nornal sinus, to sinus bradycardia, back to normal sinus. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	NormalSinus To SinusBradycardia	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Sinus Bradycardia	|		|	30	|	130	|<span class="success">	Between 40 and 60 @cite ACLS2021Bradycardia	</span>|<span class="success">	Decrease @cite ACLS2021Bradycardia	</span>|<span class="success">	Decrease @cite ACLS2021Bradycardia	</span>|<span class="success">	Little to no change	</span>|<span class="success">	Decrease @cite ACLS2021Bradycardia	</span>|<span class="success">	N/A	</span>|<span class="success">	Normal Sinus@cite ACLS2021Bradycardia	</span>|
|	Normal Sinus	|		|	210	|	300	|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|

<br><center>
*@tabledef {SinusBradycardiaAsystoleVal} Validation matrix form nornal sinus, to asystole, to sinus bradycardia. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	Asystole To SinusBradycardia	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Asystole	|		|	30	|	180	|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	Flatline/Asystole @cite ACLS2021asystole	</span>|
|	Sinus Bradycardia	|		|	30	|	130	|<span class="success">	Between 40 and 60 @ACLS2021Bradycardia	</span>|<span class="success">	Decrease @cite ACLS2021Bradycardia	</span>|<span class="success">	Decrease @cite ACLS2021Bradycardia	</span>|<span class="success">	Little to no change	</span>|<span class="success">	Decrease @cite ACLS2021Bradycardia	</span>|<span class="success">	N/A	</span>|<span class="success">	Normal Sinus@cite ACLS2021Bradycardia	</span>|


#### Sinus Tachycardia

<br><center>
*@tabledef {SinusTachyVal} Validation matrix for going from nornal sinus, to sinus tachycardia, back to normal sinus. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	NormalSinus To SinusTachycardia	|	Notes	|	Action Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Sinus Tachycardia	|		|	30	|	130	|<span class="success">	>100 @cite ACLS2021Tachy	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Increase @cite sohn2007hemodynamic	</span>|<span class="success">	Decreases as Heart Rate increases @cite aroesty1985simultaneous	</span>|<span class="success">	Normal Sinus@cite ACLS2021Tachy	</span>|
|	Normal Sinus	|		|	210	|	300	|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|


<br><center>
*@tabledef {SinusTachyAsysoleVal} Validation matrix for going from nornal sinus, to asystole, to sinus tachycardia. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	Asystole To SinusTachycardia	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Asystole	|		|	30	|	180	|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	Flatline/Asystole @cite ACLS2021asystole	</span>|
|	Sinus Tachycardia	|		|	30	|	130	|<span class="success">	>100 @cite ACLS2021Tachy	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Increase @cite sohn2007hemodynamic	</span>|<span class="success">	Decreases as Heart Rate increases @cite aroesty1985simultaneous	</span>|<span class="success">	Normal Sinus@cite ACLS2021Tachy	</span>|


#### Sinus Pulseless Electrical Activity
<br><center>
*@tabledef {PEAVal} Validation matrix for Sinus Pulseless Electrical Activity. The table shows the engine output compared to key hemodynamic and respiratory parameters*
</center>
|	Sinus Pulseless Electrical Activity To NormalSinus	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	SinusPulselessElectricalActivity	|		|	30	|	180	|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	Sinus w/ Reduced Amplitude @cite ACLS2021asystole	</span>|
|	Normal Sinus	|		|	210	|	300	|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|


#### Ventricular Fibrillation

Course and fine ventricular fibrillation only differ by the ECG waveform output.
Physiology is modelled identically for both.

<br><center>
*@tabledef {VentribularFibCoarse} Validation matrix for going from normal sinus, to coarse Ventricular Fibrillation, back to normal sinus. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	Coarse Ventricular Fibrillation To NormalSinus	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	CoarseVentricularFibrillation	|		|	30	|	180	|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	Coarse Ventricular Fibrillation @cite ClevelandClinic2021vfib	</span>|
|	Normal Sinus	|		|	210	|	300	|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|

<br><center>
*@tabledef {VentribularFibFine} Validation matrix for going from normal sinus, to fine Ventricular Fibrillation, back to normal sinus. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	Fine Ventricular Fibrillation To NormalSinus	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	FineVentricularFibrillation	|		|	30	|	180	|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	Fine Ventricular Fibrillation @cite ClevelandClinic2021vfib	</span>|
|	Normal Sinus	|		|	210	|	300	|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|


#### Ventricular Tachycardia

<br><center>
*T@tabledef {VentricularStableTachy} Validation matrix for going from normal sinus, to stable Ventricular Tachycardia, back to normal sinus. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	NormalSinus To Stable Ventricular Tachycardia	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Stable Ventricular Tachycardia	|		|	30	|	130	|<span class="success">	100-150 @cite ACLS2021Tachy	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Increase @cite sohn2007hemodynamic	</span>|<span class="success">	Decreases as Heart Rate increases @cite aroesty1985simultaneous	</span>|<span class="success">	Ventricular Tachycardia@cite ACLS2021Tachy	</span>|
|	Normal Sinus	|		|	210	|	300	|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|

<br><center>
*@tabledef {VentricularStableAsystole} Validation matrix for going from normal sinus, to asystole, to stable Ventricular Tachycardia. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	Asystole To Stable Ventricular Tachycardia	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Asystole	|		|	30	|	180	|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	Flatline/Asystole @cite ACLS2021asystole	</span>|
|	Stable Ventricular Tachycardia	|		|	30	|	130	|<span class="success">	100-150 @cite ACLS2021Tachy	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Little to no change @cite LearningNetwork2021stable	</span>|<span class="success">	Increase @cite sohn2007hemodynamic	</span>|<span class="success">	Decreases as Heart Rate increases @cite aroesty1985simultaneous	</span>|<span class="success">	Ventricular Tachycardia@cite ACLS2021Tachy	</span>|

<br><center>
*@tabledef {VentricularUnstableTachy} Validation matrix for going from normal sinus, to unstable Ventricular Tachycardia, back to normal sinus. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	NormalSinus To Unstable Ventricular Tachycardia	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Untable Ventricular Tachycardia	|		|	30	|	130	</span>|<span class="success">	>  150 @cite ACLS2021Tachy	</span>|<span class="success">	Decrease @cite Wegria1958effect	</span>|<span class="success">	Decrease @cite Wegria1958effect	</span>|<span class="success">	Decrease with reduced hemodynamic stability	</span>|<span class="danger">	Decrease @cite Wegria1958effect	</span>|<span class="success">	Decrease @cite Wegria1958effect	</span>|<span class="success">	Ventricular Tachycardia w/ reduced amplitude @cite ACLS2021Tachy	</span>|
|	Normal Sinus	|		|	210	|	300	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|

<br><center>
*@tabledef {VentricularUnstableTacyAsystole} Validation matrix for going from normal sinus, to asystole, to unstable Ventricular Tachycardia. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	Asystole To Unstable Ventricular Tachycardia	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Asystole	|		|	30	|	180	|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	0 @cite ACLS2021asystole	</span>|<span class="success">	Flatline/Asystole @cite ACLS2021asystole	</span>|
|	Untable Ventricular Tachycardia	|		|	30	|	130	</span>|<span class="success">	>  150 @cite ACLS2021Tachy	</span>|<span class="success">	Decrease @cite Wegria1958effect	</span>|<span class="success">	Decrease @cite Wegria1958effect	</span>|<span class="success">	Decrease with reduced hemodynamic stability	</span>|<span class="danger">	Decrease @cite Wegria1958effect	</span>|<span class="success">	Decrease @cite Wegria1958effect	</span>|<span class="success">	Ventricular Tachycardia w/ reduced amplitude @cite ACLS2021Tachy	</span>|


#### Pulseless Ventricular Tachycardia
<br><center>
*@tabledef {PulselessVentricularTachy} Validation matrix for Pulseless Ventricular Tachycardia arrhythmias. The table shows the engine output compared to key hemodynamic and respiratory parameters.*
</center>
|	Pulseless Ventricular Tachycardia To NormalSinus	|	Notes	|	Occurrence Time (s)	|	Sample Scenario Time (s)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Coronary Perfusion Pressure (mmHg)	|	Oxygen Saturation (mmHg)	|	Cardiac Output(mL/min)	|	Stroke Volume (mL)	|	ECG Output (mV)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	PulselessVentricularTachycardia	|		|	30	|	180	|<span class="success">	undetectable @cite ACLS2021Pulseless	</span>|<span class="success">	18 +/- 11 @cite paradis1992aortic, Significant drop @cite Babbs1983relationship, 33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	13.4 +/- 8.5 @cite paradis1990coronary 	</span>|<span class="success">	Will decrease as HR and RR stay at 0	</span>|<span class="success">	0 @cite ACLS2021Pulseless	</span>|<span class="success">	0 @cite ACLS2021Pulseless	</span>|<span class="success">	Ventricular Tachycardia w/ Reduced Amplitude @cite ACLS2021Pulseless	</span>|
|	Normal Sinus	|		|	210	|	300	|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Return to normal after period of recovery 	</span>|<span class="success">	Normal Sinus @cite ACLS2021asystole	</span>|


### Cardiopulmonary Resuscitation (CPR)
There are three CPR scenarios for validation. Each scenario perscribes the same frequency with the 3 different types of Chest compression actions. In each scenario, cardiac arrest is initiated externally. Ten seconds later, compressions begin. In each scenario, the compression rate is set to 80 per minute, and the force is set to 311 N (70 pounds) to match the conditions in @cite redberg1993physiology. Supplemental literature sources were used to validate outputs not available in @cite redberg1993physiology. All of the physiological variables were within validation ranges in both scenarios with the exception of mean arterial pressure and ejection fraction. The mean arterial pressure in the engine is slightly higher than expected. This is most likely due to the fact that the intravascular pressures are higher than those reported in @cite redberg1993physiology. However, the engine pressures are within ranges reported in other references @cite kim2008direction , @cite gruben1990system. The ejection fraction is considerably lower in the engine during CPR than the value reported in @cite kim2008direction. The engine ejection fraction is lower because blood tends to pool in the engine right heart during cardiac arrest. The validation failures that occur right at cardiac arrest are mostly due residual dynamics following asystole in the engine. Errors associated with the cessation of heart function in the engine are a known issue, and resolving this issue is a part of the cardiac arrest recommended improvements discussed [below](@ref cardiovascular-future).  

<br><center>
*@tabledef {CPR} Validation matrix for cardiopulmonary resuscitation (CPR) validation results. The table shows the engine output compared to validation data for key hemodynamic values.*
</center>
|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Heart Rate(beats/min)	|	Systolic Pressure (mmHg)	|	Diastolic Pressure (mmHg)	|	Mean Arterial Pressure (mmHg)	|	Cardiac Output (mL/min)	|	Stroke Volume (mL)	|	Carotid Artery (Brain) Flow (mL/min)	|	Ejection Fraction (%)	|	
|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	
|	Cardiac Arrest	|	30	|	35	|<span class="success">	0	</span>|<span class="success">	N/A	</span>|<span class="success">	N/A	</span>|<span class="success">	33 +/- 10 @cite kim2008direction 	</span>|<span class="success">	0	</span>|<span class="success">	0	</span>|<span class="success">	Approach 0 	</span>|<span class="success">	0	</span>|	
|	Chest Compressions	|	40	|	165-170	|<span class="success">	80 per minute [Direct calculation]	</span>|<span class="success">	39.27 @cite redberg1993physiology Approx. 70-80 @cite gruben1990system 105 +/- 41 @cite kim2008direction	</span>|<span class="success">	13.97 @cite redberg1993physiology Approx. 40 @cite gruben1990system 33 +/- 10 @cite kim2008direction 	</span>|<span class="warning">	21.13 @cite redberg1993physiology	</span>|<span class="success">	17-27% Normal @cite kim2008direction 	</span>|<span class="success">	19.7 @cite redberg1993physiology 25 +/- 8 @cite kim2008direction 	</span>|<span class="success">	> 0 during compression (translated from dog study) @cite rudikoff1980mechanisms 	</span>|<span class="success">	34% +/- 16 @cite kim2008direction	</span>|	
|	Chest CompressionsForce Scale	|	40	|	165-170	|<span class="success">	80 per minute [Direct calculation]	</span>|<span class="success">	39.27 @cite redberg1993physiology Approx. 70-80 @cite gruben1990system 105 +/- 41 @cite kim2008direction	</span>|<span class="success">	13.97 @cite redberg1993physiology Approx. 40 @cite gruben1990system 33 +/- 10 @cite kim2008direction 	</span>|<span class="warning">	21.13 @cite redberg1993physiology	</span>|<span class="success">	17-27% Normal @cite kim2008direction 	</span>|<span class="success">	19.7 @cite redberg1993physiology 25 +/- 8 @cite kim2008direction 	</span>|<span class="success">	> 0 during compression (translated from dog study) @cite rudikoff1980mechanisms	</span>|<span class="success">	34% +/- 16 @cite kim2008direction	</span>|	


@anchor hemorrhage-validation
@insert ./validation/markdown/Hemorrhage_Validation.md


### Pericardial Effusion
The pericardial effusion scenario has a chronic effusion applied to the patient with a volume accumulation on the pericardium of 500 milliliters. There is a decrease in stroke volume, arterial pressures, and cardiac output. This is due to increasing intrapericardial pressure leading to a reduction in end diastolic volume. The validation trends somewhat follow this same behavior. Pericardial effusion can also be applied as an action and the action and condition can be applied to show a worsening of the chronic condition.

<br><center>
*@tabledef {PericardialEffusionChronic} Validation matrix for a chronic case of pericardial effusion. The table shows the engine output compared to key hemodynamic parameters.*
</center>
|	Segment	|	Notes		|	Sampled Scenario Time (s)	|	Heart Rate (/min)	|	Systolic Pressure (mmHg)	|	Diastolic Pressure (mmHg)	|	Cardiac Output (mL/min)	|	Heart Stroke Volume (mL)	|	Pericardium Pressure (mmHg)	|	Pericardium Volume (mL)	|	Pulmonary Capillaries Wedge Pressure (mmHg)	|	Oxygen Saturation	|
|	------------------------	|	------------------------	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|
|	Pericardial Effusion Condition	|	Volume accumulation on pericardium is set at 500 ml.		|	120	|<span class="success">	Slight increase or no change @cite spodick1998tamp	</span>|<span class="success">	Slight decrease @cite spodick1998tamp	</span>|<span class="success">	Slight decrease @cite spodick1998tamp	</span>|<span class="success">	Decrease @cite spodick1998tamp	</span>|<span class="success">	Decrease @cite spodick1998tamp	</span>|<span class="success">	Increase @cite spodick1998tamp	</span>|<span class="success">	Increase @cite spodick1998tamp	</span>|<span class="success">	Slight increase @cite spodick1998tamp	</span>|<span class="success">	No Change 	</span>|


<br><center>
@tabledef {PericardialEffusionAcute} Validation matrix for a chronic case of pericardial effusion combined with an acute worsening of the pericardial effusion. The table shows the engine output compared to key hemodynamic parameters.*
</center>
|	Segment	|	Notes		|	Sampled Scenario Time (s)	|	Heart Rate (/min)	|	Systolic Pressure (mmHg)	|	Diastolic Pressure (mmHg)	|	Cardiac Output (mL/min)	|	Heart Stroke Volume (mL)	|	Pericardium Pressure (mmHg)	|	Pericardium Volume (mL)	|	Pulmonary Capillaries Wedge Pressure (mmHg)	|	Oxygen Saturation	|
|	------------------------	|	------------------------	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|	------------------------	|
|	Pericardial Effusion Condition	|	Volume accumulation on pericardium is set at 500 ml.		|	30	|<span class="success">	Slight increase or no change @cite spodick1998tamp	</span>|<span class="success">	Slight decrease @cite spodick1998tamp	</span>|<span class="success">	Slight decrease @cite spodick1998tamp	</span>|<span class="success">	Decrease @cite spodick1998tamp	</span>|<span class="success">	Decrease @cite spodick1998tamp	</span>|<span class="success">	Increase @cite spodick1998tamp	</span>|<span class="success">	Increase @cite spodick1998tamp	</span>|<span class="danger">	Slight increase @cite spodick1998tamp	</span>|<span class="success">	No Change 	</span>|
|	Pericardial Effusion Acute	|	Flow into pericardium set to 6 mL/min.		|	150	|<span class="success">	Slight increase or no change @cite spodick1998tamp	</span>|<span class="success">	Slight decrease @cite spodick1998tamp	</span>|<span class="success">	Slight decrease @cite spodick1998tamp	</span>|<span class="success">	Decrease @cite spodick1998tamp	</span>|<span class="success">	Decrease @cite spodick1998tamp	</span>|<span class="success">	Increase @cite spodick1998tamp	</span>|<span class="success">	Increase @cite spodick1998tamp	</span>|<span class="warning">	Slight increase @cite spodick1998tamp	</span>|<span class="success">	No Change 	</span>|


@anchor cardiovascular-conclusions
Conclusions
-----------
The CV System uses circuit methodology to simulate blood flow and physiological connection to other systems within the human body. This system provides accurate results for resting physiology and allows for the simulation of a variety of CV-related insults and interventions with appropriate system feedback. Circuit modeling of the CV System is both quick and effective, and the implementation allows the user to easily modify the circuit to vary the model resolution and fidelity or integrate their own models.

@anchor cardiovascular-future
Future Work
===========

Recommended Improvements
------------------------

An area of potential future advancement lies in the inertance of the %Cardiovascular System. The @ref CircuitMethodology has the ability to incorporate inertance into the lumped parameter models. In the future, this could be added to the %Cardiovascular Model to provide a more accurate blood pressure waveform.

Another potential area for improvement is simulation of a tourniquet. An intervention could be added to simulate the increased resistance to flow and external pressure application that would be present with the application of a tourniquet.

We will continue to add arrhythmias and the effects of defibrillation to the system.

We are exploring further discretization of the circuit; however, this must always be balanced with the speed of the computation.

The ventricular systolic function and anemia are now unvalidated conditions in the engine. We will improve the model and validate this condition in the future.

@anchor cardiovascular-appendices
Appendices
==========

Glossary
--------
AHA - American Heart Association

CFD - Computational Fluid Dynamics

CPR - Cardiopulmonary Resuscitation

CV - %Cardiovascular

%ECG - Electrocardiogram

MAP - Mean Arterial Pressure

mL - Milliliters

mmHg - Millimeters mercury

Data Model Implementation
-------------------------
@ref CardiovascularSystemTable "Cardiovascular"

@ref ElectroCardioGramTable "ElectroCardioGram"

@ref TissueSystemTable "Tissue"
