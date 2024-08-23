%Respiratory Methodology {#RespiratoryMethodology}
========================

@anchor respiratory-overview
Overview
========

@anchor respiratory-abstract
Abstract
--------

The %Respiratory System supplies oxygen and removes waste carbon dioxide from the body through a combination of ventilation and gas exchange across the blood-gas barrier (pulmonary capillary-alveoli interface). The %Respiratory System is designed to model the ventilatory behavior (both positive- and negative-pressure) of the patient %Respiratory System using electrical analogue lumped parameter models. The %Respiratory Model employs realistic pressure source signal and chemical stimuli feedback mechanisms as drivers for spontaneous ventilation. The model handles several patient conditions, including tension pneumothorax and airway obstruction. The majority of the lung values investigated for the overall model matched the validation data found in publications. Patient conditions also showed strong agreement with clinically significant output parameters, i.e., respiration rate, oxygen saturation, heart rate, and blood pressure.

@anchor respiratory-intro
## Introduction

@anchor respiratory-physiology
### Respiratory Physiology

The human %Respiratory System consists of the upper airways (region above the cricoid cartilage), the lower airways, the lungs, and the respiratory muscles. The lower airways begin at the trachea and extend to the bronchi, bronchioles, and the alveoli. At the carina, the trachea divides into two mainstem bronchi, the right and left. The bronchi bifurcate into smaller bronchioles that continue branching for up to 23 generations, forming the tracheobronchial tree that terminates with the alveoli. Alveolar ducts and alveolar sacs are the operating units of the lungs where gas exchange occurs with the pulmonary capillaries. The first several generations of airways, where no gas exchange occurs, constitute the anatomic dead space and are referred to as the conducting zone. In contrast, alveolar ducts and sacs that terminate the tracheobronchial tree are referred to as the respiration zone.

@htmlonly
<center>
<a href="./Images/Respiratory/RespiratorySystemDiagram.png"><img src="./Images/Respiratory/RespiratorySystemDiagram.png"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {RespiratorySystemDiagram}. The %Respiratory System consists of the upper and lower airways. The diaphragm acts as a respiratory muscle taking part in the ventilatory driver mechanics. The trachea branches into the right and left bronchi, each of which further bifurcates into multiple generations of smaller bronchioles. These bronchioles form the tracheobronchial tree, which terminates at the alveoli. @cite LadyofHats2014Respiratory </i>
</center><br>

The alveolar-capillary gas exchange is facilitated by the ventilation process, which is driven by the intercostal muscles, the diaphragm, and the chest wall recoil. These mechanisms work in tandem to actively drive fresh air into the lungs and passively remove gases from the lungs. Attached to the chest wall is a thin layer of membrane (pleura) that folds back onto itself, forming two layers, known as the visceral and parietal pleurals. The pleural cavity is filled with fluid. The pressure in this space, known as the intrapleural pressure, is normally slightly below the atmospheric pressure. Even when no inspiratory muscles are contracting, the mechanical interaction between the lung and the chest wall pulls the two pleural membranes apart, resulting in a slightly decreased intrapleural pressure (-3 cm H<SUB>2</SUB>O to -5 cm H<SUB>2</SUB>O) @cite Levitzky2013pulmonary .

@anchor respiratory-math
### Mathematical Model

Mathematical modeling of the respiratory physiology dates back to the work published by Gray in 1945 @cite gray1951pulmonary . Gray provided the first mathematical description for the chemical control of pulmonary ventilation. Later, Gordins et al. developed the first dynamic model of the respiratory system in 1954 @cite grodins1954respiratory . Several mathematical models followed after that, including the work by Guyton and collaborators in 1965 @cite milhorn1965mathematical and others ( @cite grodins1967mathematical , @cite khoo1982factors , @cite saunders1980breathing , @cite ursino2004interaction ). Many of the published models describe a specific aspect of the respiratory physiology in considerable detail. To name a few, Lorandi et al. @cite lorandi2003parametric employed a mathematical model to describe the mechanical properties of the lungs, Murray et al. @cite murray1977techniques described the gas exchange properties of the lungs, Wiberg et al. @cite wiberg1979modeling and Bache et al. @cite bache1981time described the effect of higher levels of CO<SUB>2</SUB> or anesthetic gases on breathing, and Whipp et al. @cite whipp1995obligatory developed a mathematical model to describe the respiratory anaerobiosis in skeletal muscle.

Many mathematical models of mechanical ventilation employ the lumped parameter model that represents the entire ventilation process with a small number of unknowns. The simplest lumped parameter model of mechanical ventilation assumes the conducting zone can be identified with a pipe that connects a collection of alveoli to the atmosphere and exerts pneumatic resistance to the flow. This type of model can be solved with a low computational cost, which reduces runtime. For whole body models/simulations, this is an important requirement. The disadvantage of lumped parameter models can lie in the large number of parameters that can result from required circuit parameters. It is important to identify the key features and behaviors of any model to intelligently reduce the number of required parameters.

The most important parameters in the lumped parameter model of mechanical
ventilation correspond to the elastic behavior of the lung and the flow
resistance of the airways. The thoracic cage and the lung tissue exhibit an
elastic behavior that can be represented with a single compliance or multiple
compliances. The compliance *C* is calculated by taking the ratio of the volume <i>&delta;V</i>
and the pressure <i>&delta;P</i> variations as:

\f[C=\frac{\delta V}{\delta P} \f] 
<center>
<i>@equationdef {compliance}.</i>
</center><br> 

As a first-order approximation, the volume of the functional unit can be
approximated as:

\f[V(P+\delta P)=V(P)+C\delta P\f] 
<center>
<i>@equationdef {volume}.</i>
</center><br> 

In the %Respiratory System, the main source of flow resistance arises from the
flow of air through the branches in the conducting zone. Mathematical models
using the lumped parameter model select functional units for these regions and
designate the variable <i>R</i> for pneumatic flow resistance. The pressure drop <i>&Delta;P</i> across
the respiratory tree can thus be calculated by using Ohm's law analogue as

\f[\Delta P=RQ\f] 
<center>
<i>@equationdef {deltaP}.</i>
</center><br> 

where <i>Q</i> is the volumetric flow rate. The above relation assumes the flow
is laminar and the gas is incompressible. For laminar, viscous, and
incompressible flow, the Hagen-Poiseuille equation relates the pressure drop <i>&Delta;P</i>
in a fluid flowing through a cylindrical pipe of length <i>l</i> and radius <i>r</i> as

\f[\Delta P=\frac{8\mu l}{\pi r^{4} } Q\f] 
<center>
<i>@equationdef {hagen}.</i>
</center><br> 

where <i>&Mu;</i> is the dynamic viscosity. By defining the flow resistance <i>R</i> as

\f[R=\frac{8\mu l}{\pi r^{4} } \f] 
<center>
<i>@equationdef {resistance}.</i>
</center><br> 

a relation analogous to Ohm's law can be derived.

@anchor respiratory-systemdesign
System Design
=============

@anchor respiratory-background
Background and Scope
--------------------

### Previous Research

The %Respiratory Model has its roots in the mathematical model of
Yashuri Fukui and N. Ty Smith @cite FukuiSmith1981hybrid . The researchers
developed a lumped parameter mathematical model to describe the uptake and
distribution of halothane. Their %Respiratory Model consisted of two pulmonary
compartments corresponding to the dead space and the alveoli @cite FukuiSmith1981hybrid. 
The %Respiratory Model in the engine is an
extension of the work by Fukui and Smith. This model was developed and released
by Advanced Simulation Corporation as part of the simulator, Body Simulation for
Anesthesia&trade;. This later formed the backbone of the HumanSim&trade; physiology engine @cite Clipp2012Humansim . 
The basic elements of the legacy %Respiratory System were advanced by the BioGears program before being forked and
further developed and improved to allow realistic mechanical responses to pathological conditions.

### Approach

The current version of the %Respiratory Model represents the two lungs
and associated airways as five major functional units, or compartments, that are
designated as the carina, right and left anatomic dead space, and right and
left alveoli. In the model, the carina compartment represents the anatomical region
from the airway at the trachea. The right and left anatomic dead
space compartments represent the bronchi and their branching bronchioles that
are part of the conducting airways below the carina. The right and left alveoli
compartments correspond to the collection of alveoli where gas exchange occurs
between the airways and the %Cardiovascular System. The right and left chest wall
compartments represent the right and left sides of the thoracic wall. The new model additionally accounts for the pleural
cavity through circuit elements that allow flow into the pleural space in the
event of respiratory insults that involve gas leak either from the alveoli or
the thoracic wall. To account for flow through the esophagus, an incidence that
may occur during mechanical ventilation (positive-pressure ventilation), the
model provides subordinate compartments representing the esophageal passage and
the stomach. The model also consists of a pressure signal generator representing
the respiratory muscle pressure source driver.

@anchor respiratory-dataflow
Data Flow
---------

The %Respiratory System determines its state at every time step through a three
step process: Preprocess, Process, and Postprocess. In general, Preprocess
determines the circuit element values based on feedback mechanisms and engine
settings/actions. Process uses the generic circuit calculator to compute the
entire state of the circuit and fill in all pertinent values. Postprocess is
used to advance time.

### Initialize

At the beginning of a simulation, patient parameters are used to modify the muscle (pressure source) driver functionality to achieve the specified values at the end of the resting stabilization period - see the @ref respiratory-variability "Patient Variability" section for more details. After resting stabilization is achieved, any user-selected conditions are implemented to reach a new homeostatic point - see the @ref respiratory-conditions "Conditions" section for more details.

### Preprocess

#### Update Compliances

The chest wall compliances of the left and right pleural space are modified as a function of volume.

#### Process Actions and Conditions

There are several methods that modify respiratory parameters based on insults and interventions. This includes combined effects that change deadspace volumes, airway and bronchi resistances, alveolar compliances, inspiratory-espiratory ratios, diffusion surface area, pulmonary capillary resistance, aerosol deposition, and air leaks.

#### Respiratory Driver

The respiratory muscle pressure source that drives spontaneous ventilation is
calculated based on chemical stimuli feedback control mechanism.

### Process

The generic circuit methodology developed for the engine is used to
solve for the pressure, flow, and volume at each node or path. For more details
on this, see the @ref CircuitMethodology. Substance volumes and volume
fractions (concentrations) are also calculated during this step. See the @ref SubstanceTransportMethodology for more details.

The Calculate Vital Signs function uses the circuit pressure, flow, and volume
to calculate important system-level quantities for the current time step.

### Postprocess

The Postprocess step moves values calculated in the Process step from the next
time step calculation to the current time step calculation. This allows all
other systems access to the information when completing their Preprocess
analysis during the next time step.

@anchor respiratory-features
Features and Capabilities
-------------------------

### Respiratory Circuit

The %Respiratory System designates a set of functional elements, or
compartments, to model mechanical ventilation. The functional elements are
represented by an electric analogue circuit comprised of resistors, capacitors,
switches, diodes, and power sources. The latter represents the driving pressure
from the respiratory muscles. The resistors and capacitors represent the
resistance to flow through the airways and the elastic nature of the airways,
alveoli, and thoracic walls.

The equivalent of an electric switch is used to transition between different
ventilation conditions or flow pathways. For example, the electric analogue switch
accounts for incidents that permit flow through the esophageal tract while
prohibiting flow through the trachea. Unidirectional flow in the respiratory
system is handled through electric analogue diodes that allow flow in one
direction, preventing flow in the opposite direction. Such functional elements
are employed to represent insults that allow unidirectional gas flow into the
pleural cavity through an opening at the alveoli or the thoracic wall.

In the circuit model, the carina and right and left dead spaces are
represented by resistors to account for pneumatic resistance that impedes flow
of gas across the conducting zones. Each of the right and left
alveoli compartments are represented by a combination of resistors and
capacitors (compliances) to account for the elastic behavior of the alveoli. The
right and left chest wall compartments are represented by variable compliance
that allows flexibility to mechanical insults. Based on the electrical circuit
analogue, the model predicts the dynamic properties of the %Respiratory System.
@figureref {RespiratoryCircuit} depicts the network of respiratory circuit elements and their
interconnections.

@htmlonly
<center>
<a href="./Images/Respiratory/RespiratoryCircuit.png"><img src="./Images/Respiratory/RespiratoryCircuit.png" width="650"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {RespiratoryCircuit}. Circuit diagram of the %Respiratory System. The diagram depicts a
closed circuit of the major compartments and the subordinate compartments. The circuit depicts the muscle
pressure source that serves as the driver for the %Respiratory System, with larger efforts modeled as higher pressures. Unless changed
for insults and interventions, the subordinate compartments have "infinitely"
large resistors and behave as open electrical switches.</i>
</center><br>

The respiratory circuit employs circuit nodes and paths to represent
physiological state variables belonging to the %Respiratory System's functional
units. In this representation, the pressures across the compartmental units are
designated to the nodes, while all other variables (flow, volume, hydraulic
resistances, and compliances) are assigned to the paths on the circuit. At any
instant of time, the flow <i>Q</i> on a path across a resistor <i>R</i> can be calculated
using the pressure difference <i>&Delta;P</i> between the nodes across the path as <i>Q=&Delta;P/R</i>.
Similarly, the volume change <i>&Delta;V</i> of a respiratory element with compliance <i>C</i> can
be calculated based on the pressure difference <i>&Delta;P</i> between the nodes connected by
the path as <i>&Delta;V=C&Delta;P</i>. The time evolution of the pressures at each node in the
circuit is solved using the %Circuit Solver as described in the @ref
CircuitMethodology.

@anchor respiratory-variability
### Patient Variability

Several patient parameters are set/calculated outside of the %Respiratory System at the beginning of a simulation (See @ref PatientMethodology).  The patient parameters that are used as inputs to the %Respiratory System are:
- Respiration Rate Baseline: used to set the driver frequency
- Functional Residual Capacity: used to set the driver default pressure
- Total Lung Capacity: used to set the driver maximum allowable pressure
- Right Lung Ratio: used in the scaling equation for inspiratory-expiratory ratio for conditions
- Basal Metabolic Rate: used for metabolic effects
- Vital Capacity: used to determine the tidal volume plateau in the driver piecewise target alveolar ventilation function

Several patient parameters are updated at the end of each stabilization segment (Resting, Conditions, and Feedback).  This allows the simulation to reach new homeostatic points that take into account the whole-body state based on both internal and external factors.  The patient parameters that are reset by the %Respiratory System are:
- Respiration Rate Baseline: from Respiration Rate system data value
- Tidal Volume Baseline: from Tidal Volume system data value
- Functional Residual Capacity: from calculated instantaneous value
- Vital Capacity: calculated as [TLC - RV]
- Expiratory Reserve Volume: calculated as [FRC - RV]
- Inspiratory Reserve Volume: calculated as [TLC - FRC - TV]
- Inspiratory Capacity: calculated as [TLC - FRC]

The patient Alveoli Surface Area is also modified when condition/action effects are applied.

@anchor respiratory-feedback
### Feedback

#### Spontaneous Breathing Muscle Pressure Source

The %Respiratory System interacts with other systems in the engine to
receive feedback and adjust spontaneous breathing for homeostasis. To
accurately model the respiratory response under various physiological and
pathological conditions, a robust %Respiratory Model that responds to mechanical
stresses and chemical stimuli is required. To this end, the engine
%Respiratory System employs a time-dependent pressure source based on a chemical
feedback mechanism that mimics the respiratory response to blood gas levels as
sensed by the central and peripheral chemoreceptors. The pressure source
represents the muscle effort and serves as an input power source
to drive the inspiration and expiration phases of the breathing cycle. 

During inhalation, the driver pressure source is set to a negative value. The end of the exhalation cycle represents the initial
conditions of free breathing, where the alveolar pressure equals the atmospheric
pressure and no air flows
into the lungs. When the inspiratory muscles are not contracting, the mechanical
interaction between the lungs and the chest wall creates a subatmospheric intrapleural
pressure. The value of the
driver pressure in the model is selected to meet the unstressed condition at the
pleural node. In the case of mechanical ventilation, the anesthesia machine
controls the pressure at the airway node for positive pressure ventilation. More
details on positive pressure ventilation can be found in the @ref AnesthesiaMachineMethodology.

For a realistic muscle pressure source signal, the %Respiratory System
adopted a piecewise logarithmic mathematical model for each lung
based on clinical data. The single breath waveform segments are defined by fraction of total breath and are broken out by the following segments:

1. Inspiratory rise
2. Inspiratory hold
3. Inspiratory released
4. Inspiratory to expiratory pause
5. Expiratory rise
6. Expiratory hold
7. Expiratory release
8. Residue

Each segment is given as a fraction of the total breath, with all summing to a value of 1 whole breath. The inpsiratory rise (inhale) period (segment 1: <i>T<sub>ins</sub></i>) and inspiratory release (exhale) period (segment 3: <i>T<sub>exp</sub></i>) is calculated as a fraction of the total breath period (<i>T<sub>tot</sub></i>) knowing the muscle driver ventilation frequency (<i>f<sub>v</sub></i>) by @cite Fresnel2014musclePressure,

\f[\frac{{{T_{ins}}}}{{{T_{tot}}}} = 0.0125\left( {{f_v}[bpm] + 4} \right) + 0.125\f]
<center>
<i>@equationdef {tb1}.</i>
</center><br> 

\f[\frac{{{T_{\exp }}}}{{{T_{tot}}}} = {T_{tot}} - {T_{ins}}\f]
<center>
<i>@equationdef {tb2}.</i>
</center><br> 

Unless a conscious respiration action is called, all other segment fractions are set to 0. The inspiratory-expiratory ratio will change based on the driver respiration rate. The time series(<i>t</i>) of the respiratory muscle pressure (<i>P<sub>mus</sub></i>) is based on empirical data @cite kondili2010estimation as,

\f[{P_{mus}} = \left\{ {\begin{array}{*{20}{l}}
{{P_{\min }} \cdot \sin \left( {\frac{\pi }{2} \cdot \frac{t}{{{t_1}}}} \right),}&{0 < t \le {t_1}}\\
{{P_{\min }},}&{{t_1} < t \le {t_2}}\\
{{P_{\min }} \cdot \sin \left( {\frac{\pi }{2} \cdot \frac{{t + {t_3} - 2{t_2}}}{{{t_3} - {t_2}}}} \right),}&{{t_2} < t \le {t_3}}\\
{0,}&{{t_3} < t \le {t_4}}\\
{{P_{max}} \cdot \sin \left( {\frac{\pi }{2} \cdot \frac{{t - {t_4}}}{{{t_5} - {t_4}}}} \right),}&{{t_4} < t \le {t_5}}\\
{{P_{max}},}&{{t_5} < t \le {t_6}}\\
{{P_{max}} \cdot \sin \left( {\frac{\pi }{2} \cdot \frac{{t + {t_7} - 2{t_6}}}{{{t_7} - {t_6}}}} \right),}&{{t_6} < t \le {t_7}}\\
{0,}&{{t_7} < t \le {t_{\max }}}
\end{array}} \right.\f]

<center>
<i>@equationdef {pmus}.</i>
</center><br> 

Where <i>P<sub>min</sub></i> is the largest negative pressure value during inhalation and <i>P<sub>max</sub></i> is the largest positive pressure value during exhalation, the combination of which specifies the amplitude of the pressure source signal. Each time value (<i>t</i> with a subscript) is determined using set fractions and the total breath time to achieve the desired inspiratory-expiratory ratio.  @figureref {DriverWaveform} shows the basic segmented muscle driver waveform used.

@htmlonly
<center>
<a href="./plots/Respiratory/DriverWaveform.jpg">
<img src="./plots/Respiratory/DriverWaveform.jpg" width="600"></a>
<br> 
</center>
@endhtmlonly
<center>
<i>@figuredef {DriverWaveform}. Conscious respiration muscle pressure waveform showing all segments with a duration of 1s each to minimum and maximum pressure.  These segment functions are presented in the equation above. The fraction of each segment duration compared to the total breath duration is set based on the inspiratory-expiratory ratio - many often set to zero. The total time of each breath is determined from a target respiration rate.</i>
</center><br>

At the beginning of each breath, a target volume (i.e., tidal volume) is determined and mapped to the <i>P<sub>min</sub></i> value using simple circuit math and assuming constant lung and chest wall compliances. This is given by,

\f[{P_{min }} = \frac{{ - V + FRC}}{{{C_{total}}}}\f]
<center>
<i>@equationdef {targetV}.</i>
</center><br> 

Where <i>V</i> is the target volume, <i>FRC</i> is the functional residual capacity and <i>C<sub>total</sub></i> is the total compliance of the respiratory system. The total compliance is determined from the baseline constant compliances of the left chest wall (<i>C<sub>LCW</sub></i>), left lung (<i>C<sub>LL</sub></i>), right chest wall (<i>C<sub>RCW</sub></i>), and right lung (<i>C<sub>RL</sub></i>) by,

\f[{C_{total}} = \frac{1}{{\frac{1}{{{C_{LCW}}}} + \frac{1}{{{C_{LL}}}}}} + \frac{1}{{\frac{1}{{{C_{RCW}}}} + \frac{1}{{{C_{RL}}}}}}\f]
<center>
<i>@equationdef {totC}.</i>
</center><br> 

##### Chemoreceptors
@anchor respiratory-chemoreceptors

The Fresnel model uses pre-selected ventilation frequencies to model various physiological and pathological conditions. The %Respiratory System extended the Fresnel, et. al. model by incorporating a chemical stimuli feedback mechanism that contributes to the overall blood gas regulation. As a chemical feedback mechanism, past works used empirical relationships between minute ventilation, <i>V<sup><b>.</b></sup><sub>E</sub></i>, or alveolar ventilation, <i>V<sup><b>.</b></sup><sub>A</sub></i>, and the blood gas partial pressures that represent the respiratory response to chemical stimuli at the peripheral and central chemoreceptors @cite Khoo1982chemicalFeedback , @cite Batzel2005chemicalFeedback . The %Respiratory Model adopted the mathematical relation  that links the alveolar ventilation with the blood gas levels. The resulting mathematical relationship implemented in the %Respiratory System is 

\f[\dot{V}_{A} =G_{p} e^{-0.05P_{a} O_{2} } \max (0,P_{aCO_{2} } -I_{p} )+G_{c} \max (0,P_{aCO_{2} } -I_{c} )\f] 
<center>
<i>@equationdef {fresnel}.</i>
</center><br> 

<i>P<sub>aO</sub><sub>2</sub></i> and <i>P<sub>aCO</sub><sub>2</sub></i> are the arterial oxygen and carbon dioxide partial pressures, respectively. <i>I<sub>p</sub></i> and <i>I<sub>c</sub></i> are the cutoff threshold constants, and <i>G<sub>p</sub></i> and <i>G<sub>c</sub></i> are the peripheral and central controller gain constants, respectively. The value of <i>f<sub>v</sub></i> used in the pressure source corresponds to the target  respiration rate that the engine must attain to ensure accurate blood gas levels. It is related to the minute ventilation, <i>V<sup><b>.</b></sup><sub>E</sub></i>, as shown in the equation below

\f[f_{v} =\dot{V}_{E} /V_{T} \f] 
<center>
<i>@equationdef {fV}.</i>
</center><br> 

where <i>V<sup><b>.</b></sup><sub>E</sub></i> is calculated using the relation

\f[\dot{V}_{E} =\dot{V}_{A} +\dot{V}_{D} \f] 
<center>
<i>@equationdef {ve1}.</i>
</center><br> 

<i>V<sup><b>.</b></sup><sub>D</sub></i> is the dead space ventilation and is obtained by taking the product of the dead space volume and the respiration rate. The target tidal volume <i>V<sub>T</sub></i> needed to predict <i>f<sub>v</sub></i> is calculated from the pulmonary ventilation based on a piecewise linear relationship between the tidal volume and the minute ventilation as shown by Watson @cite watson1974tidalVolume . In the article, the author presented data that  describes the relationship between the minute ventilation and tidal volume by straight line. The data is reproduced in @figureref {Respiratory_Figure05} below.

@htmlonly
<center>
<a href="./Images/Respiratory/Respiratory_Figure05.png">
<img src="./Images/Respiratory/Respiratory_Figure05.png" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Respiratory_Figure05}. The figure shows data from literature that presents the linear relationship between the minute ventilation 
and tidal volume. The relationship was derived from a line fit of experimental data with a wide range of varying carbon dioxide, exercise, postures, alveolar gas tensions, adrenaline, mild acidaemia, alkalosis, morphine, mederidine, mild hypoxia, and breathing through a small fixed resistance.  All test cases matched this trend of two intersecting straight lines.  The figure is reproduced from @cite watson1974tidalVolume .</i>
</center><br> 

The data in the article shows that the minute ventilation 
can be described by two intersecting straight lines. 
Up to about half of the vital capacity <i>V<sub>C</sub></i>, the minute ventilation,  
<i>V<sup><b>.</b></sup><sub>E</sub></i>, and the tidal volume, <i>V<sub>T</sub></i>, are related as

\f[\dot{V}_{E} =m(V_{T} -c)\f] 
<center>
<i>@equationdef {ve2}.</i>
</center><br> 

where <i>m</i> is the slope and <i>c</i> is the x-intercept of the minute ventilation versus tidal volume plot. The data 
shows that the minute ventilation is constant above half of the vital capacity. Based on this observation, the 
%Respiratory Model employs the linear relation given below to predict the target tidal 
volume from the minute ventilation.

\f[V_{T} =\left\{\begin{array}{l} {c+\dot{V}_{E} /m,V_{T} \le V_{C} } \\ {0.5*V_{C} ,V_{T} >V_{C} } \end{array}\right. \f] 
<center>
<i>@equationdef {vr}.</i>
</center><br> 

Where <i>m</i> and <i>c</i> are constant parameters determined during initialization. During the initial parameterization, the minute ventilation is plotted against the vital capacity to determine <i>c</i> by taking the x-intercept of <i>V<sup><b>.</b></sup><sub>E</sub></i> vs <i>V<sub>T</sub></i> plot. Then, the slope is adjusted to meet the initial baseline parameters of the patient. These parameters include the baseline respiration rate and tidal volume, where the latter is estimated from the patient's weight. 

Once <i>m</i> and <i>c</i> are selected this way, they are used as patient parameters for determining the target tidal volume from the minute ventilation that is determined by the feedback mechanism. This provides the target tidal volume that must be attained to respond to the chemical stimuli feedback. This value is then communicated back to the target ventilation frequency <i>f<sub>v</sub></i>, which in turn adjusts the patient's breathing frequency through the muscle pressure. The muscle pressure then drives sufficient gas into the lungs in response to the chemical stimuli, completing the feedback loop. 

In the calculation of the target tidal volume, the %Respiratory Model allows the target tidal volume to increase linearly with the calculated minute ventilation as long as the target volume is below half the vital capacity. In the event that the calculated target volume is above half of the vital capacity, the target volume is set to a constant value of 0.5V<sub>C</sub> as shown in the above equation. In the model, the lung vital capacity V<sub>C</sub> is calculated from the patient's total lung capacity and residual volume as described in the sections below.

The model described above is implemented in the engine with reference values and model parameters that are tuned to meet validation data. The reference and tuned values for the model parameters are shown in @tableref {driver} below. Note that our model is tuned to use the reference values without modification.

<center>
<i>@tabledef {driver}. %Respiratory driver model parameters and reference values used in the chemical feedback mechanism implementations.</i>
|Parameter (units)                        |Reference Value                           |Model Value             |
|------------------------                 |------------------------                  |------------------------|
|G<sub>p</sub>                            |30.24 @cite Batzel2005chemicalFeedback    |30.24                   |
|G<sub>c</sub>                            |1.44  @cite Batzel2005chemicalFeedback    |1.44                    |
|I<sub>p</sub>, I<sub>c</sub>(mmHg)       |35.5  @cite Batzel2005chemicalFeedback    |35.5                    |
|P<sub>0.1</sub>(cmH<SUB>2</SUB>O)        |0.75  @cite Budwiser2007chemicalFeedback  |0.75                    |
</center><br>

##### Mechanoreceptor Feedback Mechanism

The respiratory muscle driver pressure is dynamically adjusted based on pulmonary mechanoreceptor feedback. This method reduces the inspiratory drive and respiratory muscle pressure during a breath by inhibiting the inspiratory effort through mechanoreceptor feedback, similar to the Hering-Breuer Reflex. The driver pressure is instantaneously scaled down in response to lung inflation caused by assisted positive pressure ventilation. This approach reduces the instantaneous pressure source value to align with chemoreceptor targets.

##### Spontaneous Breathing Results

@figureref {pressures} depicts the time-dependent driver pressure source of the %Respiratory System as obtained during simulation of the standard patient model of the engine (77 kg adult male) under normal physiological conditions. For comparison, the driver pressure is plotted with the alveolar, intrapleural, and transpulmonary pressures. The figure shows the pressures for several breathing cycles. The model driver pressure exhibits distinct waveforms during the inspiration and expiration phases. These patterns represent the active distension and passive relaxation behaviors of the inspiratory muscles. As a result of such input, the model distinguishes between the active inspiratory and passive expiratory phases of the breathing cycle. The time-dependent muscle pressure together with the atmospheric pressure and the compliances act in tandem to generate the pleural and alveolar pressure waveforms shown in the figure.

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/Muscle_Pressure.jpg"><img src="./plots/Respiratory/Muscle_Pressure.jpg" width="800"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/Pleural_and_Alveoli_Pressure.jpg"><img src="./plots/Respiratory/Pleural_and_Alveoli_Pressure.jpg" width="800"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/Transpulmonary_Pressure.jpg"><img src="./plots/Respiratory/Transpulmonary_Pressure.jpg" width="800"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {pressures}. The driver pressure, or pressure source, that serves as an electrical analogue voltage source for the respiratory circuit is plotted along with the alveolar, intrapleural, and transpulmonary pressures.  The pressure source generates a subatmospheric intrapleural pressure that facilitates the inspiration and expiration phases of spontaneous breathing.</i>
</center><br>

#### Compliances

The Pulse respiratory system is separated into four compliances (see the circuit diagram in Figure 6) defined by the left and right chest walls and lungs. The pressure-volume relationship has been well studied in describing the mechanical behavior of the lungs during inflation and deflation @cite harris2005pressure. A comprehensive sigmoidal equation for the entire system has been determined from empirical pulmonary pressure-volume data @cite venegas1998comprehensive. This compliance curve has been further broken into two constant values for the left and right lung curves and two sigmoidal functions for the left and right lungs. @figureref {ComplianceCurve} shows the right side (combined chest wall and lung) compliance curve for the healthy standard patient. This function is varied based on patient settings. During simulations, the instantaneous compliances based on this curve are determined using the current lung volume.

@htmlonly
<center>
<a href="./Images/Respiratory/ComplianceCurve.png">
<img src="./Images/Respiratory/ComplianceCurve.png" width="550"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {ComplianceCurve}. The healthy single lung compliance curve is determined by standard patient lung volume parameters and a baseline compliance (C<sub>baseline</sub>) value.</i>
</center><br>

The waveform in @figureref {ComplianceCurve} is defined by these mathematical relationships,

\f[V = RV + \frac{{VC}}{{1 + {e^{{{ - \left( {P - c} \right)} \mathord{\left/
 {\vphantom {{ - \left( {P - c} \right)} d}} \right.
 \kern-\nulldelimiterspace} d}}}}}\f]
<center>
<i>@equationdef {ccVolume}.</i>
</center><br> 
 
\f[{P_{cl}} = c - 2d\f]
<center>
<i>@equationdef {ccPcl}.</i>
</center><br> 

\f[{P_{cu}} = c + 2d\f]
<center>
<i>@equationdef {ccPcu}.</i>
</center><br> 

Where (<i>V</i>) is the individual lung volume, (<i>P</i>) is the intrapulmonary pressure, and the other variables are defined in @figureref {ComplianceCurve}. These equations can be rearranged and input with known parameters to determine the instantaneous expected pressure (<i>P</i>) of each lung. First, the baseline side compliance (<i>C<sub>sb</sub></i>) is determined knowing the baseline chest wall (<i>C<sub>cwb</sub></i>) and baseline lung (<i>C<sub>lb</sub></i>) compliances,

\f[{C_{sb}} = \frac{1}{{\frac{1}{{{C_{cwb}}}} + \frac{1}{{{C_{lb}}}}}}\f]
<center>
<i>@equationdef {baseC}.</i>
</center><br> 

The expected intrapulmonary pressure (<i>P</i>) at the a given volume (<i>V</i>) can be calculated knowing the individual lung functional residual capacity (<i>FRC</i>), residual volume (<i>RV</i>), and vital capacity (<i>VC</i>) by the following,

\f[\lambda  = \ln \left( {\frac{{FRC - RV}}{{RV + VC - FRC}}} \right)\f]
<center>
<i>@equationdef {lambda}.</i>
</center><br> 

\f[{P_{cu}} = \frac{{VC - FRC}}{{{C_{sb}}}}\f]
<center>
<i>@equationdef {iPcu}.</i>
</center><br> 

\f[c =  - \frac{{{P_{cu}}\lambda \left( {2 - \lambda } \right)}}{2}\f]
<center>
<i>@equationdef {ic}.</i>
</center><br> 

\f[d = \frac{{{P_{cu}} - c}}{2}\f]
<center>
<i>@equationdef {id}.</i>
</center><br> 

\f[P = d \cdot \ln \left( {\frac{{V - RV}}{{RV + VC - V}}} \right) + c\f]
<center>
<i>@equationdef {iP}.</i>
</center><br> 

Then, the instantaneous chest wall compliance (<i>C<sub>cw</sub></i>) to apply at the current timestep is found using the side compliance (<i>C<sub>s</sub></i>) by,

\f[{C_s} = \frac{{V - FRC}}{P}\f]
<center>
<i>@equationdef {Cs}.</i>
</center><br> 

\f[{C_{cw}} = \frac{1}{{\frac{1}{{{C_s}}} - \frac{1}{{{C_{lb}}}}}}\f]
<center>
<i>@equationdef {Ccw}.</i>
</center><br> 

#### Standard Lung Volumes and Capacities

There are a number of standard lung volumes and capacities that are measured
during different stages of normal and deep breathing cycles. The inspiratory
reserve volume (IRV), tidal volume (V<sub>T</sub>), expiratory reserve volume (ERV), and
residual volume (RV) correspond to the four standard lung volumes. The
inspiratory capacity (IC), forced residual capacity (FRC), vital capacity (V<sub>C</sub>),
and total lung capacity (TLC) are the four standard lung capacities that consist
of two or more standard lung volumes. These volumes and capacities are good
diagnostics for lung functionality, and the %Respiratory Model reports
their values as outputs. As mentioned above, some of the parameters are obtained
from patient data as input variables. The TLC and FRC are two of these
parameters that are drawn from the patient data. For the standard patient in the
model, TLC and FRC are set to be 6.0 L and 2.4 L, respectively. Using these
parameters as inputs, the engine calculates the other standard lung volumes and
capacities as described below.

##### Residual volume (RV)

The residual volume is the volume of gas remaining in the lungs after maximal
exhalation. As mentioned above, the %Respiratory Model approximates the residual volume
based on the patient weight (RV = 16.0 mL/kg) (@cite Corning2007pulmonary , 
@cite Rennie2013pulmonary ). 
For the standard patient in the model 
with 77 kg weight, the residual volume
RV=1.23 L. Typical values of RV vary in the literature. For example, for 70 kg
patients: RV=1.5 L @cite Levitzky2013pulmonary , 1.2 L @cite silverthorn2013human, 
and 1.682 L @cite stocks1995reference . The 
engine employs weight-based relation and the values used in the engine are close 
to those found in the literature @cite silverthorn2013human .

##### Expiratory Reserve Volume (ERV)

ERV is the maximum volume below the tidal volume that can be expired during
maximal forced expiration. ERV can be calculated as

\f[ERV=FRC-RV\f] 
<center>
<i>@equationdef {ERV}.</i>
</center><br> 

In this equation, both FRC and RV are input values obtained from weight-based
relation. For the standard patient in the model (77 kg adult male), FRC=2.31 L, and
RV=1.23 L, thus ERV becomes ERV=1.08 L. Typically, reported values for 
ERV are around 1.1 L @cite guyton2006medical .

##### Tidal volume (V<sub>T</sub>)

Tidal volume corresponds to the volume of air inspired or expired in a single
breathing cycle during normal quiet breathing. For a healthy 70 kg adult, the
tidal volume is 540 ml per breath. The tidal volume can be calculated by
numerically integrating the volumetric flow rate of inspired air flowing through
the trachea. The %Respiratory Model calculates the tidal volume by
taking the difference between the maximum and minimum total lung volumes during
each breathing cycle.

@figureref {TidalVolume_from_TotalLungVolume} depicts the typical lung volume waveform for multiple breathing
cycles. The %Respiratory Model outputs the value of V<sub>T</sub> for each
breathing cycle. @figureref {TidalVolume_from_TotalLungVolume} presents the plot of the total lung volume and V<sub>T</sub> as a
function of time.

@htmlonly
<center>
<a href="./plots/Respiratory/TidalVolume_from_TotalLungVolume.jpg">
<img src="./plots/Respiratory/TidalVolume_from_TotalLungVolume.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {TidalVolume_from_TotalLungVolume}. This shows the relationship of the total lung volume with the tidal
volume. The tidal volume for each cycle is determined by taking the difference between the maximum and
minimum values of the total lung volume, and is therefore only updated at the end of each full cycle.</i>
</center><br>

##### Inspiratory Reserve Volume (IRV)

IRV is the additional volume, above the tidal volume, that can be inspired
during maximal forced inspiration. IRV can be calculated from total lung
capacity (TLC) using the relation

\f[IRV=TLC-FRC-V_{T} \f] 
<center>
<i>@equationdef {IRV}.</i>
</center><br> 

Both TLC and FRC are weight-based inputs to the model, whereas V<sub>T</sub> is calculated
as described above. Using TLC=6.16 L (i.e., 80 mL/kg) and FRC=2.31 L (i.e., 30 mL/kg) of
the standard patient in the model, IRV becomes 3.307 L for an average V<sub>T</sub> of
543 mL, where the average V<sub>T</sub> is calculated by taking the time average of V<sub>T</sub>
described above. Using weight-based tidal volume of V<sub>T</sub> = 7 mL/kg @cite Levitzky2013pulmonary , IRV 
can be shown to be 3.31 L for 77 kg patient, in good agreement with the value from 
the model.

##### Vital Capacity (V<sub>C</sub>)

V<sub>C</sub>  is the volume of air that can be expired after maximal inspiration. V<sub>C</sub> can be
calculated as

\f[V_{C} =IRV+V_{T} +ERV\f] 
<center>
<i>@equationdef {VsubC}.</i>
</center><br> 

V<sub>C</sub>  can also be calculated using TLC as:

\f[V_{C} =TLC-RV\f] 
<center>
<i>@equationdef {TLC_VsubC}.</i>
</center><br> 

Again, both TLC and RV are weight-based inputs to the model, and V<sub>T</sub> is calculated
as described above. For the standard patient in the model with TLC=6.16 L (80
L/kg) and RV=1.23 L, V<sub>C</sub>=4.93 L. Typical values of V<sub>C</sub>  reported in 
the literature are around 4.6 L @cite silverthorn2013human . The value in the engine corresponds to 
a weight-based vital capacity.

##### Inspiratory Capacity (IC)

The inspiratory capacity is another standard lung capacity that can be
calculated from TLC and FRC as

\f[IC=TLC-FRC\f] 
<center>
<i>@equationdef {IC}.</i>
</center><br> 

In the model, both TLC and FRC are weight-based input variables, and IC can be
calculated using the above equation. From the values in the model, IC can be
determined to be 3.85 L.

#### Ventilation

##### Respiration Rate (RR)

As described above, the %Respiratory Model employs chemical feedback
mechanisms to regulate the ventilation frequency that affects the breathing cycle
through the respiratory driver. The breathing frequency is adjusted in
accordance to the arterial O<SUB>2</SUB> and CO<SUB>2</SUB> levels and other modifiers, such as drug and metabolic effects.
The engine switches between the inspiratory and expiratory phases based on the
predicted ventilation frequency. The respiration rate is then calculated by
measuring the time taken for a complete breathing cycle and converting it to the
number of breaths per minute. Typically, the respiration rate of a healthy adult
is 16 breaths/min @cite Levitzky2013pulmonary . A similar value is obtained
for the standard patient under normal tidal breathing.

##### Total Pulmonary Ventilation

The total pulmonary ventilation (or minute ventilation or minute volume) is the
volume of air moved into the lungs per minute. Minute ventilation (V<sup><b>.</b></sup><sub>E</sub>) is the
product of tidal volume (V<sub>T</sub>) and respiration rate (RR), i.e.,

\f[\dot{V}_{E} =V_{T} *RR\f] 
<center>
<i>@equationdef {dotV}.</i>
</center><br> 

The %Respiratory Model calculates both V<sub>T</sub> and RR from the simulation data. 
V<sup><b>.</b></sup><sub>E</sub> can thus be determined from the above equation by using the average values of V<sub>T</sub> and
RR. For the standard patient in the model, under normal physiological conditions,
the average values of V<sub>T</sub> and RR are 0.540 L and 16 breaths/min,
respectively. The total pulmonary ventilation obtained from the model equals
6.53 L/min. Typical weight-based value of minute volume is 84 mL/min/kg @cite Levitzky2013pulmonary . 
Using the mass 
of the standard patient (77 kg), the expected value of V<sup><b>.</b></sup><sub>E</sub> is 6.48 L/min, which is close to 
the value found from the engine.

##### Alveolar Ventilation

The term alveolar ventilation corresponds to the volume of air entering and
leaving the alveoli per minute @cite Levitzky2013pulmonary . The alveolar
ventilation is calculated as V<sup><b>.</b></sup><sub>A</sub>=(V<sub>T</sub>-V<sub>D</sub>)\*RR where V<sup><b>.</b></sup><sub>A</sub> is the alveolar
ventilation in liters per minute and V<sub>D</sub> is the volume of the conducting airways
that is referred to as the anatomical dead space. This is the region of
respiratory tract where no gas exchange takes place. In the %Respiratory Model, the volume of the dead space is calculated from the values
assigned to the right and left anatomic dead space nodes. These nodes have continuously changing volumes due the compliances that are connected to the
nodes. The right and left anatomic dead space volumes when
compared to the right and left alveoli volumes are shown in @figureref {Alveoli_and_Dead_Space_Volumes}. When the patient weight is factored into the 
calculation, the alveolar ventilation predicted from the model is close to the expected value.

@htmlonly
<center>
<a href="./plots/Respiratory/Alveoli_and_Dead_Space_Volumes.jpg">
<img src="./plots/Respiratory/Alveoli_and_Dead_Space_Volumes.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Alveoli_and_Dead_Space_Volumes}. The right and left anatomic dead space volumes together with the right and left 
alveoli volumes. The difference in the alveoli volumes is due to the difference in 
 the lung ratio of the right and left lungs. The right and left lung ratios of the standard patient 
 in the %Respiratory Model are 0.525 and 0.475, respectively. The left and right dead space volumes are equivalent.</i>
</center><br>

##### Tracheal Airflow

Airflow is measured by taking the instantaneous pressure difference across a
fixed resistance. The %Respiratory Model measures tracheal airflow
<i>Q<sub>trachea</sub></i> by using the instantaneous pressure difference across the tracheal
resistance <i>R<sub>trachea</sub></i> as:

\f[Q_{trachea} =\frac{P_{mouth} -P_{carina} }{R_{trachea} } \f] 
<center>
<i>@equationdef {Qtrachea}.</i>
</center><br> 

<i>P<sub>mouth</sub></i> and <i>P<sub>carina</sub></i> are the pressures at the mouth and the carina nodes,
respectively. The figure shown below presents the absolute flow rate (no distinction for flow direction) for one breathing cycle.

@htmlonly
<center>
<a href="./plots/Respiratory/Total_Flow_Through_Trachea.jpg">
<img src="./plots/Respiratory/Total_Flow_Through_Trachea.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Total_Flow_Through_Trachea}. Tracheal airflow and total lung volume during one typical breathing
cycle. At the peak of the inspiration phase, the flow rate goes to zero.</i>
</center><br>

#### Alveolar Pressure

The instantaneous pressures at the nodes of the respiratory circuit are
calculated by solving the circuit matrix equation as described in the @ref
CircuitMethodology. The pressures at the right and left alveoli nodes of the
%Respiratory Model represent the alveolar pressure. Typically, the values of
the alveolar pressure vary in the range from -1.8 cm H<SUB>2</SUB>O to 1.8 cm H<SUB>2</SUB>O (relative
to atmospheric pressure) during the inspiration and expiration phases of the
breathing cycle @cite otis1947measurement . The figure
below depicts the alveolar pressure along with lung
volume for one breathing cycle. The alveolar pressure in the engine is absolute (not relative
to atmospheric pressure), so the relative pressure can be determined by subtracting the standard atmospheric pressure of 1033 cmH2O - giving outputs close to the range of  -1.8 cm H<SUB>2</SUB>O to 1.8 cm H<SUB>2</SUB>O
found in the literature @cite otis1947measurement . 

@htmlonly
<center>
<a href="./plots/Respiratory/Lung_Pressure_And_Volume.jpg">
<img src="./plots/Respiratory/Lung_Pressure_And_Volume.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Lung_Pressure_And_Volume}. Typical lung pressures. The plot shows the instantaneous pressure of
the left alveoli for one breathing cycle. For comparison, the plot also
shows the total lung volume for the same breathing cycle. As seen in this figure,
the lung volume increases as the alveolar pressure falls below the atmospheric pressure of 1033 cm H<SUB>2</SUB>O.
This creates a pressure differential between the airway node and the alveoli,
allowing air to flow into the lungs. When the alveolar pressure goes above 
1033 cm H<SUB>2</SUB>O, the lung volume decreases from its peak, representing the
expiration phase.</i>
</center><br>

##### Transpulmonary pressure

Transpulmonary pressure is defined as the difference between the alveolar 
 and the intrapleural pressures. The %Respiratory System derives the transpulmonary 
 pressure from the calculated values of the alveolar pressure and intrapleural pressures. 
 The alveolar pressure is obtained by taking the average of the left and right 
 alveolar pressures. Similarly, the total intrapleural pressure is obtained by taking the average of the 
 the left and right pleural pressures. The plots shown below compare the transpulmonary pressure from the 
 engine with that found in literature @cite guyton2006medical. Some variations in the waveforms and 
 possibly the average values is a consequence of the specific driver pressure and 
 patient parameters employed in the engine.

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/Engine_Pressures.jpg"><img src="./plots/Respiratory/Engine_Pressures.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/Guyton_Lung_Pressures.jpg"><img src="./plots/Respiratory/Guyton_Lung_Pressures.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/Engine_Lung_Volume.jpg"><img src="./plots/Respiratory/Engine_Lung_Volume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/Guyton_Lung_Volume.jpg"><img src="./plots/Respiratory/Guyton_Lung_Volume.jpg" width="550"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Guyton_Lung_Pressures}. A plot showing the transpulmonary pressure obtained from the engine with 
that found and digitized from literature @cite guyton2006medical. The left plots use absolute pressure, while the right Guyton plots use the pressure difference from ambient (1033 cmH2O).  For comparison, the plot also shows the lung volumes from the two sources.</i>
</center><br>

#### Pressure-Volume (P-V) curve

One method of characterizing the lungs' elastic behavior is to use a diagram
that relates the lung volume changes to changes in pleural pressure. The pressure-volume 
curve of a healthy person shows hysteresis during the inspiratory and expiratory phases. 
@figureref {Guyton_Pulmonary_Compliance} presents the pressure-volume diagram of data extracted from the 
%Respiratory Model. For comparison, the plot also shows a P-V diagram reproduced from literature
@cite guyton2006medical . The figures show the plot of lung volume changes versus pleural pressure
 for one breathing cycle. The pleural pressure from the model is the average of the right and left pleural 
 pressures. The lung volume change corresponds to the change in the total lung volume  
 during a complete breathing cycle.  
As shown in the figure, the %Respiratory Model mimics the expected 
hysteresis of the P-V curve.

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/Engine_Pulmonary_Compliance.jpg"><img src="./plots/Respiratory/Engine_Pulmonary_Compliance.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/Guyton_Pulmonary_Compliance.jpg"><img src="./plots/Respiratory/Guyton_Pulmonary_Compliance.jpg" width="550"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Guyton_Pulmonary_Compliance}. The pressure-volume curve for the standard patient of the %Respiratory Model 
under normal physiological conditions. For comparison, the figure includes plot reproduced from 
literature @cite guyton2006medical . The plot from the model shows the expected hysteresis of the P-V 
diagram observed in a healthy person.</i>
</center><br>

#### Partial Pressures of Respiratory Gases

For any gas mixture, the partial pressure P<sub>gas</sub> of a particular gas in the
mixture can be calculated based on the total pressure P<sub>total</sub> of all gases in the
mixture and the fractional concentration F<sub>gas</sub> of the gas as

\f[P_{gas} =F_{gas} *P_{total} \f] 
<center>
<i>@equationdef {Pgas}.</i>
</center><br> 

The %Respiratory Model calculates the partial pressure of a gas at any node
based on the total pressure and the volume fraction of the gas at the node. The
node volume fraction of the gas and the node pressure are calculated in a manner
described in the @ref SubstanceTransportMethodology and @ref CircuitMethodology,
respectively. The %Respiratory Model employs the above equation to predict the
partial pressure of gases in the %Respiratory System. The sections below present
the results of alveolar and tracheal partial pressures of O<SUB>2</SUB> and CO<SUB>2</SUB>.

##### Alveolar O<SUB>2</SUB> Partial Pressure

The engine calculates the O<SUB>2</SUB> partial pressure P<sub>Lung<sub>O<SUB>2</SUB></sub/></sub/> at each
alveoli node by using the oxygen volume fraction VF<sub>Lung<sub>O<SUB>2</SUB></sub/></sub/> and the total
pressure P<sub>Lung</sub/> at the alveoli nodes as

\f[P_{LungO_{2} } =VF_{LungO_{2} } *P_{Lung} \f] 
<center>
<i>@equationdef {P_LungO2_1}.</i>
</center><br> 

The alveolar O<SUB>2</SUB> partial pressure can thus be determined by taking the average of
O<SUB>2</SUB> partial pressures at the two alveoli nodes. In the model, the alveoli node
pressures are gauge pressures and are expressed relative to the atmospheric
pressure. The model converts these relative pressures to their absolute
pressures when calculating the gas partial pressures. The engine
assumes the inspired air is heated and humidified. Therefore, the water vapor
pressure at normal body temperature (P<sub>H<SUB>2</SUB>O</sub>=47 mm Hg) is subtracted from the
standard atmospheric pressure of P<sub>B</sub>=760 mm Hg when the gas partial pressure is
calculated using the absolute lung pressure, i.e.,

\f[P_{LungO_{2} } =VF_{LungO_{2} } *(P_{B} -P_{H_{2} O} +P_{Lung} )\f] 
<center>
<i>@equationdef {P_LungO2_2}.</i>
</center><br> 

@figureref {Alveolar_Oxygen_Partial_Pressure} depicts the plot of P<sub>LungO<SUB>2</SUB></sub>  for the left and right alveoli
of the standard patient. Typically, the average alveolar partial pressure of oxygen 
 is 104 mmHg @cite Levitzky2013pulmonary . The value from the engine is close to that of the 
 literature.

@htmlonly
<center>
<a href="./plots/Respiratory/Alveolar_Oxygen_Partial_Pressure.jpg">
<img src="./plots/Respiratory/Alveolar_Oxygen_Partial_Pressure.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Alveolar_Oxygen_Partial_Pressure}. Alveolar O<SUB>2</SUB> partial pressure. The partial pressure of O<SUB>2</SUB> at the two
alveoli nodes is calculated by using the pressure and the O<SUB>2</SUB> volume fraction at
each alveoli node. The plot shows the value of O<SUB>2</SUB> partial pressure as a function
of time over the course of multiple breathing cycles.</i>
</center><br>

##### Alveolar CO<SUB>2</SUB> Partial Pressure

The alveolar CO<SUB>2</SUB> partial pressure is calculated in the same manner as the oxygen
partial pressure. @figureref {Alveolar_Carbon_Dioxide_Partial_Pressure} depicts the plot of alveolar CO<SUB>2</SUB> partial
pressure for the left and right alveoli nodes. Typically, the average alveolar CO<SUB>2</SUB> partial pressure is 40 mmHg @cite Levitzky2013pulmonary .
The prediction from the engine is close to the expected
literature value.

@htmlonly
<center>
<a href="./plots/Respiratory/Alveolar_Carbon_Dioxide_Partial_Pressure.jpg">
<img src="./plots/Respiratory/Alveolar_Carbon_Dioxide_Partial_Pressure.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Alveolar_Carbon_Dioxide_Partial_Pressure}. Alveolar CO<SUB>2</SUB> partial pressure. The partial pressure of CO<SUB>2</SUB> at the two
alveoli is calculated by using the pressure and the CO<SUB>2</SUB> volume fraction at each
alveoli node. The plot shows the value of the CO<SUB>2</SUB> partial pressure as a function
of time over the course of multiple breathing cycles.</i>
</center><br>

##### Tracheal CO<SUB>2</SUB> Partial Pressure

Tracheal CO<SUB>2</SUB> partial pressure is calculated in the same manner as the alveolar
CO<SUB>2</SUB> partial pressure. The engine calculates the tracheal partial pressure by
using the pressure at the carina node. Recall that the carina node belongs to
the node where the trachea branches into the mainstem bronchi. The figure below
depicts the time variation of tracheal CO<SUB>2</SUB> partial pressure. The waveform for
CO<SUB>2</SUB> partial pressure appears similar to those found in normal capnograph at the mouth
(Fig.3.9, Ref @cite Levitzky2013pulmonary ). 

@htmlonly
<center>
<a href="./plots/Respiratory/Trachea_Carbon_Dioxide_Partial_Pressure.jpg">
<img src="./plots/Respiratory/Trachea_Carbon_Dioxide_Partial_Pressure.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Trachea_Carbon_Dioxide_Partial_Pressure}. Tracheal CO<SUB>2</SUB> partial pressure. The partial pressure of CO<SUB>2</SUB> at the
trachea is calculated by using the pressure and the CO<SUB>2</SUB> volume fraction at the
carina node. The plot shows the value of tracheal CO<SUB>2</SUB> partial pressure over the course of one breathing cycle.</i>
</center><br>

##### Tracheal O<SUB>2</SUB> Partial Pressure

The O<SUB>2</SUB> partial pressure at the trachea is calculated in the same manner as the
alveolar O<SUB>2</SUB> partial pressure. As mentioned for CO<SUB>2</SUB> partial pressure, the model
calculates the tracheal O<SUB>2</SUB> partial pressure by making use of the pressure at the
carina node. The output of the tracheal O<SUB>2</SUB> partial
pressure calculation is presented in @figureref {Trachea_Oxygen_Partial_Pressure}.

@htmlonly
<center>
<a href="./plots/Respiratory/Trachea_Oxygen_Partial_Pressure.jpg">
<img src="./plots/Respiratory/Trachea_Oxygen_Partial_Pressure.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Trachea_Oxygen_Partial_Pressure}. Tracheal O<SUB>2</SUB> partial pressure. The partial pressure of O<SUB>2</SUB> at the
trachea is calculated by using the pressure and the O<SUB>2</SUB> volume fraction at the
carina node. The plot shows the value of tracheal O<SUB>2</SUB> partial pressure over the course of one breathing cycle.</i>
</center><br>

#### Recruitment Shunting

Recruitment plays a critical role in oxygenation by determining the extent to which alveoli are open and able to participate in gas exchange. The recruitment of alveoli directly impacts the degree of pulmonary shunting, where blood bypasses areas of the lung that are not ventilated, leading to reduced oxygenation.

In the Pulse Physiology Engine, lung recruitment is modeled based on acinar ventilation and volume, which in turn influences the pulmonary shunt fraction. This approach is calibrated using empirical data that reflects changes observed with varying levels of positive end-expiratory pressure (PEEP) during mechanical ventilation. For more details on this calibration process, refer to the @ref MechanicalVentilatorMethodology.

The concept of the Recruited Fraction is central to this model. It represents the fraction of lung volume below the functional residual capacity (FRC) compared to the residual volume. At or above FRC, the recruited alveoli are fully functional, resulting in a shunt resistance multiplier of 1, meaning no additional shunting occurs. However, as lung volume decreases toward the residual volume, the recruited fraction diminishes, leading to a resistance multiplier that approaches 0, which corresponds to maximum shunting.

This relationship is visually depicted in @figuredef {recruitment}, which illustrates the mapping of reduced alveolar volume to the shunt scaling factor across different cardiopulmonary compartments.

@htmlonly

<center>
<a href="./plots/Respiratory/RecruitmentShunting.png">
<img src="./plots/Respiratory/RecruitmentShunting.png" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {recruitment}. The mapping of reduced alveolar volume to shunt scaling factor for each cardiopulmonary compartment.</i>
</center><br>

@anchor respiratory-dependencies
### Dependencies

The %Respiratory System interacts with other physiological
systems either directly or indirectly through processes that involve the
transport and exchange of gases. These interdependencies are discussed below.

#### Gas Transport

The transport of gases in the %Respiratory Model is handled through
the Transport functionality of the engine, where mass conservation based
on volume fraction and volumetric flow rate of gases at the nodes and paths of
the respiratory circuit is employed (see the @ref SubstanceTransportMethodology for
details). During free breathing, the ambient atmospheric volume fractions are used as an input to the %Respiratory Model.
 
By using methods described in the @ref SubstanceTransportMethodology, the 
engine calculates the volume fraction of gases at the nodes associated with the
respiratory functional units. The %Respiratory System uses the calculated volume
fractions and predicts various physiological parameters. For example, the gas volume
fraction at the end of expiration, referred to as end-tidal gas concentration,
can be monitored based on the results from the @ref SubstanceTransportMethodology
calculation. The end-tidal gas concentration is an important clinical parameter
for monitoring patients and preventing mishaps related to insufficient
ventilation or inappropriate gas concentration during anesthesia and immediate
recovery @cite linko1989monitoring. Monitoring end tidal CO<SUB>2</SUB> (ETCO<SUB>2</SUB>) is a
widely established clinical practice for verification of endotracheal tube
placement and is also one of the standard requirements for monitoring patients
in transport @cite donald2006end. The %Respiratory Model provides the
end-tidal gas concentration based on the expired gas volume fractions at the
airway node.

#### Environment

The %Respiratory System interacts with the %Environment System for
the atmospheric pressure values assigned to the mouth node. Changes to the 
environmental conditions, such as changes in altitude, ambient temperature, humidity, 
and others, can affect the breathing pattern of the patient. The %Respiratory System 
interacts with the %Environment System to utilize the values of 
ambient pressures and gas concentrations that reflect the environmental condition.
The %Respiratory System also interacts with the %Environment System for proper handling 
of inhaled gases that can arise from environmental incidents.

#### Alveolar Gas Exchange

At the alveoli-capillary interface, gas transfer occurs. The primary goal is to
transfer incoming oxygen into the bloodstream for transport to other organs and
to transfer waste carbon dioxide out of the body. This mechanism is also used to
transport inhaled agents into the bloodstream from the
%Respiratory System. For more details on gas transport, see @ref
SubstanceTransportMethodology and @ref BloodChemistryMethodology.

In the %Respiratory Model, the spontaneous respiration rate is adjusted based on a chemical feedback mechanism
that depends on the arterial oxygen and carbon dioxide levels as described
above. The arterial oxygen and carbon dioxide levels depend on the level of O<SUB>2</SUB>
consumption and CO<SUB>2</SUB> production in the circulatory system, which in turn affects
the gas exchange at the alveolar-capillary interface. As the arterial O<SUB>2</SUB> and CO<SUB>2</SUB>
levels change, the breathing rate, tidal volume, and alveolar ventilation also change,
which in turn facilitates efficient gas exchange between the atmosphere and the
body. This presents an example of how the engine integrates the
circulatory and %Respiratory Systems to regulate the blood gas levels.

#### Drug Effects

As drugs circulate through the system, they affect the %Respiratory System. The drug effects on respiratory rate and tidal volume are calculated in the %Drugs System. These effects are applied to the respiratory driver by modifying the frequency and amplitude of the breathing cycle. During the respiratory driver calculations, the respiratory rate and tidal volume changes that are calculated in the %Drugs System are applied to the resulting respiratory rate and tidal volume calculated in the driver. Additionally, a neuromuscular block level is applied based on the drug effects.  If the neuromuscular block level is above 0.15 on a scale of 0 to 1.0, then the respiratory rate and tidal volume are set to zero. This represents the paralytic effects of a neuromuscular block agent.  The value of 0.15 was chosen to satisfy the block duration for succinylcholine and rocuronium. The strength of these effects, including the block effect, are calculated based on the plasma concentration and the drug parameters in the substance files in the %Drugs System.  For more information on this calculation see @ref DrugsMethodology.

#### Metabolic Effects

A metabolic modifier is set by the %Energy System (@ref EnergyMethodology) to drive the system to reasonable levels achievable during increased metabolic exertion.  The modifier is tuned to achieve the correct respiratory response for near maximal exercise, and a linear relationship is assumed. This modifier is a direct multiplier to the target alveolar ventilation input into the system driver, and it causes an increase in both tidal volume and respiration rate.

#### Equipment Connection

The %Respiratory System can be hooked up to various equipment, including the anesthesia machine (see the @ref AnesthesiaMachineMethodology) and mechanical ventilator (see the @ref MechanicalVentilatorMethodology), for positive-pressure ventilation, as well as an inhaler model (see the @ref InhalerMethodology) for aerosol delivery. The connection of each piece of equipment is achieved by linking the respective fluid circuits. The connection nodes for the equipment are connected to the respiratory mouth node to allow for automatic calculation of fluid mechanics by the circuit solver and transport by the substance transporter. The mechanistic cascading effects are automatically achieved, and every connection is simulated as part of the overall respiratory system model.

@anchor respiratory-outputs
### Outputs

At each time iteration, the %Circuit Solver calculates the values of the
state variables for that particular time. Using the calculated state variables,
the model predicts various physiological parameters of mechanical ventilation. Many of the calculated system data outputs are derived from the difference between the minimum and maximum lung volumes and the time between occurances - e.g., tidal volume and respiration rate.

Other values, like the pulmonary resistance and compliance, are determined instantaneously.  The pulmonary resistance is calculated by taking the ratio of 
the pressure difference between the mouth <i>P<sub>mouth</sub></i> and the alveoli <i>P<sub>alveoli</sub></i> and 
the flow across the trachea <i>Q<sub>trachea</sub></i> as

\f[R_{pulm} =\frac{P_{mouth} -P_{alveoli} }{Q_{trachea} } \f] 
<center>
<i>@equationdef {Rplum}.</i>
</center><br> 

The %Respiratory Model calculates the pulmonary compliance <i>C<sub>pulm</sub></i> by dividing the tidal 
volume <i>V<sub>T</sub></i> by the intrapleural pressure <i>P<sub>pleu</sub></i> difference as

\f[C_{pulm} =\frac{V_{T} }{P_{pleau(max )} -P_{pleu(min )} } \f] 
<center>
<i>@equationdef {Cplum}.</i>
</center><br> 
 
Here <i>P<sub>pleu(min)</sub></i> and <i>P<sub>pleu(max)</sub></i> are the minimum and maximum respective pressures at the 
right and left pleural nodes.

@anchor respiratory-assumptions
Assumptions and Limitations
---------------------------

### Assumptions

As in most lumped parameter models of mechanical ventilation, the 
%Respiratory Model is based on two main parameters: the resistance R and the
compliance C (or elastance E). Similar to their electrical analogues, these
elements form a closed circuit to represent the energy dissipation and storage
properties of the normal tidal ventilation. One extension of the linear model
is the addition of inertance in the lumped parameter model. Inertance is
not included in the model on the assumption that inertia does not play a
significant role under conventional tidal breathing, as opposed to high-frequency
ventilation.

In the model, the %Respiratory System is assumed to behave linearly in that the
hydraulic resistance R is assumed to be independent of the flow rate Q, and the
elastance E or the compliance C of the elastic component is assumed to be
independent of the volume V. Therefore, under normal physiological conditions,
the circuit elements (resistors and capacitors) of the %Respiratory
System are treated as constants. However, their values can be adjusted when
addressing pathological conditions.

The %Respiratory Model does not distinguish between different functional units
in the upper airway, the anatomical region where the inspired air is heated,
humidified, and filtered. In this region, the flow of air is turbulent, as
opposed to the laminar flow in lower airways. That means that the Ohm's analogue
pressure-volume relation cannot be applied to this region. However, the model
treats the upper airway as part of the mouth node and assumes laminar,
incompressible flow for the remaining airway.

### Limitations

The current version of the %Respiratory Model does not account for the upper
airways, and the detailed nature of upper airway flows will not be captured in
the model. It is known that flows at the upper airways are turbulent, while flows
in the lower respiratory tract are laminar. The absence of upper airways in
the model can underestimate the airway flow. In this regard, the lumped
parameter model is inherently at a disadvantage in handling complex geometries
and flows that can represent detailed upper airway pathology. However, the model
is sufficiently detailed to reproduce the respiratory responses associated with a
number of airway pathologies at relatively low computational cost.

Airway inertance is assumed to be negligible at normal respiratory frequencies.
However, analysis of high-frequency ventilation (HFV) of up to 40 Hz ventilatory
frequency would require the inclusion of inertance @cite rozanek2008design .
In mechanical ventilation, HFV is a useful procedure that permits significantly
decreased pressure amplitude and tidal volume, preventing trauma in artificial
ventilation. When compared to conventional mechanical ventilation, HFV is
considered to be the preferred method of mechanical ventilation in patients with
acute lung injury @cite Krishnan2000hfv . The small tidal volumes needed in
HFV are thought to offer an advantage over conventional ventilation in limiting
additional lung injury from mechanical ventilation @cite Krishnan2000hfv . The
current version of the %Respiratory Model handles conventional mechanical
ventilation but not HFV.

The model makes no distinction between different generations of bifurcating
airways. Therefore, factors affecting the regional ventilation and perfusion of
the lungs cannot be captured by the model.

Insults and Interventions
----------

### General Approach

In the Pulse Physiology Engine, disease states are modeled by systematically modifying a range of physiological parameters within the simulated patient. For chronic conditions, the engine stabilizes the patient to a new homeostatic baseline before initiating the simulation, ensuring a realistic depiction of the disease's progression and its impact on the body's systems over time.

Pulse is equipped to simulate both restrictive and obstructive pulmonary diseases, with each condition's severity represented through distinct continuous function mappings. These mappings are informed by extensive literature review, ensuring that the simulated disease parameters align closely with clinically observed data. As illustrated in @tablered {diseases}, parameter settings for various representative conditions and severities are based on trends and values derived from the following references: @cite brunner2019lung, @cite arnal2018parameters, @cite harris2005pressure, @cite aguirre2018lung, @cite arndt1995linear, @cite bikker2008end, @cite brunner2012pulmonary, @cite ibanez1982normal.

To capture the full spectrum of respiratory pathophysiology, Pulse applies disease-specific parameter modifiers determined by a severity index, ranging from 0 (no disease) to 1 (maximum severity). These severities are mapped to corresponding modifiers through piecewise linear functions, calibrated at key transition points: 0.0 (no change), 0.3 (mild), 0.6 (moderate), 0.9 (severe), and 1.0 (maximum). Depending on the nature of the disease and its physiological impact, these modifiers are applied either as a multiplier or as an incremental adjustment to the relevant physiological parameters, as detailed in @tabledef {diseases}.

A key strength of this approach is the ability to simulate the cumulative effects of comorbid conditions by combining or stacking multiple modifiers. This feature allows for a more comprehensive representation of how various diseases interact within the body, particularly in complex scenarios involving multiple chronic conditions.

For chronic obstructive pulmonary disease (COPD), the engine applies separate severity indices for bronchitis and emphysema, reflecting the distinct pathophysiological mechanisms at play in each condition. These severities are mapped independently, allowing for nuanced control over the progression of COPD within the simulated patient.

Alveolar modifiers are applied individually to each cardiopulmonary compartment, ensuring that the localized effects of diseases like acute respiratory distress syndrome (ARDS) are accurately represented. In the case of ARDS, the shunting effect is determined by the greater of the recruitment factor (see the Recruitment Shunting section above) or the ARDS severity, allowing for dynamic adjustments as the disease progresses.

The engine also accounts for changes in dead space volume, referencing these adjustments to the baseline healthy lung volumes of the standard patient. This ensures that the physiological implications of increased dead space, such as reduced gas exchange efficiency, are accurately captured.

When positive pressure ventilation is introduced, the engine models the resulting changes in the respiratory circuit's resistance and compliance, in line with findings from @cite arnal2018parameters. For intubated patients, these modifiers are stacked and combined with other active modifiers to reflect the compounded effects of the disease state and the intervention.

<center><br>
<i>@tabledef {diseases}. This table outlines the property changes due to the application of various respiratory diseases and positive pressure ventilation in the Pulse Physiology Engine. ARDS and COPD are applied by defining a severity between 0 and 1. Healthy severity = 0.0, mild severity = 0.3, moderate severity = 0.6, severe severity = 0.9, maximum severity = 1.0.</i>
</center>

<table>
    <tr>
        <th>Parameter</th>
        <th>Healthy</th>
        <th>Positive Pressure Ventilation</th>
        <th colspan="4">Restrictive (ARDS)</th>
        <th colspan="4">Obstructive (COPD)</th>
    </tr>
    <tr>
        <th></th>
        <th></th>
        <th></th>
        <th>Mild</th>
        <th>Moderate</th>
        <th>Severe</th>
        <th>Maximum</th>
        <th>Mild</th>
        <th>Moderate</th>
        <th>Severe</th>
        <th>Maximum</th>
    </tr>
    <tr>
        <td>Airway Resistance Multiplier</td>
        <td>1</td>
        <td>15</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
    </tr>
    <tr>
        <td>Bronchi Inhale Resistance Multiplier</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>10</td>
        <td>60</td>
        <td>100</td>
        <td>150</td>
    </tr>
    <tr>
        <td>Bronchi Exhale Resistance Multiplier</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>35</td>
        <td>120</td>
        <td>180</td>
        <td>250</td>
    </tr>
    <tr>
        <td>Alveoli Resistance Multiplier</td>
        <td>1</td>
        <td>1</td>
        <td>10</td>
        <td>15</td>
        <td>20</td>
        <td>25</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
    </tr>
    <tr>
        <td>Alveoli Compliance Multiplier</td>
        <td>1</td>
        <td>0.45</td>
        <td>0.65</td>
        <td>0.55</td>
        <td>0.50</td>
        <td>0.40</td>
        <td>1.06</td>
        <td>1.08</td>
        <td>1.09</td>
        <td>1.20</td>
    </tr>
    <tr>
        <td>Pulmonary Shunt Resistance Multiplier</td>
        <td>1</td>
        <td>1</td>
        <td>0.100</td>
        <td>0.200</td>
        <td>0.150</td>
        <td>0.100</td>
        <td>0.280</td>
        <td>0.181</td>
        <td>0.220</td>
        <td>0.100</td>
    </tr>
    <tr>
        <td>Alveolar Dead Space Increment</td>
        <td>0</td>
        <td>0</td>
        <td>0.12</td>
        <td>0.14</td>
        <td>0.18</td>
        <td>0.20</td>
        <td>0.02</td>
        <td>0.05</td>
        <td>0.12</td>
        <td>0.20</td>
    </tr>
    <tr>
        <td>Diffusion Surface Area Multiplier</td>
        <td>1</td>
        <td>1</td>
        <td>0.50</td>
        <td>0.20</td>
        <td>0.10</td>
        <td>0.05</td>
        <td>0.50</td>
        <td>0.20</td>
        <td>0.10</td>
        <td>0.05</td>
    </tr>
    <tr>
        <td>Pulmonary Capillary Resistance Multiplier</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>2.5</td>
        <td>4</td>
        <td>5.50</td>
        <td>6</td>
    </tr>
    <tr>
        <td>Spontaneous Effort Multiplier</td>
        <td>1</td>
        <td>1</td>
        <td>0.76</td>
        <td>0.52</td>
        <td>0.28</td>
        <td>0.20</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
        <td>1</td>
    </tr>
    <tr>
        <td>Spontaneous Inspiratory-Expiratory Ratio Multiplier</td>
        <td>1</td>
        <td>1</td>
        <td>1.30</td>
        <td>1.60</td>
        <td>1.90</td>
        <td>2.00</td>
        <td>0.85</td>
        <td>0.70</td>
        <td>0.55</td>
        <td>0.50</td>
    </tr>
</table>

Modifications to respiratory circuit resistances and compliances can further be examined and validated through volume-flow curves, like those created during spirometry testing. @figureref {FlowVolumeCurves} shows results from a simulated pulmonary function test with the standard patient healthy and with moderate ARDS and COPD. The conscious respiration action was used to exhale to the expiratory reserve volume and inhale to the inspiratory capacity.

@htmlonly
<center>
<a href="./plots/Respiratory/FlowVolumeCurves.jpg">
<img src="./plots/Respiratory/FlowVolumeCurves.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {FlowVolumeCurves}. A spirometry simulation comparing healthy, obstructive (COPD), and restrictive (ARDS) forced breath loops. COPD and ARDS severities of 0.6 and conscious respiration actions are used in the simulation. Results match those expected and presented in literature @cite barreiro2004approach.</i>
</center><br>

@htmlonly
<center>
<a href="./plots/Respiratory/SpirometryVolumeCurves.jpg">
<img src="./plots/Respiratory/SpirometryVolumeCurves.jpg" width="800"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {SpirometryVolumeCurves}. Volume vs. time results from the same pulmonary function test as @figureref {FlowVolumeCurves}. This scenario uses the COPD and ARDS exacerbation action to simulate all three modalities with the conscious respiration action to complete a full exhale-inhale cycle. Changes in the functional residual capacity, residual volume, and total lung capacity can be seen at the maximum and minimum points. Results match those expected and presented in literature @cite kliegman2007nelson.</i>
</center><br>


@anchor respiratory-conditions
### Conditions

#### Chronic Obstructive Pulmonary Disease

Chronic Obstructive Pulmonary Disease (COPD) is an obstructive lung disease characterized by chronically reduced airflow into the lungs. Symptoms typically include increased respiration rate, decreased  tidal volume, decreased expiratory flow, and reduced oxygen saturation @cite man2003contemporary . Unlike asthma, the reduction in airflow does not generally improve with medication. Symptoms typically worsen over time, often leading to death. COPD is caused by damage to the small airways in the lungs and destruction of the alveolar membranes. When tissue damage occurs primarily in the airways, the condition is called chronic bronchitis. When tissue destruction is focused in the alveoli, the condition is called emphysema. In the engine, COPD is a manifestation of both conditions, although one condition may dominate. 

The engine simulates COPD by modeling damage to the small airways and alveolar membranes. COPD severity is controlled by two severity values, a chronic bronchitis severity value and an emphysema severity value. Chronic bronchitis severity is used to scale the airflow resistance through the lower airways in the circuit model, simulating airway tissue damage and scarring. Increasing chronic bronchitis severity increases the airflow resistance through the lower airways. The function used to determine airflow resistance for COPD is the same as that used for asthma and is based on chronic bronchitis severity.

Emphysema severity is used to scale destruction of the alveolar membranes. Destruction of the alveolar membranes decreases the effective surface area for gas exchange, thereby reducing alveolar gas transfer. In cases of severe emphysema, up to 80% of the alveolar membranes are destroyed, with a corresponding reduction in effective gas diffusion surface area @cite guyton2006medical. The engine scales the gas diffusion surface area using a multiplier based on emphysema severity.

Additionally, The engine models destruction of lung tissue with an increase in alveolar compliance.

The destruction of the alveolar membranes also destroys the pulmonary capillaries embedded in the membranes. To model pulmonary capillary destruction, the engine increases the pulmonary capillary flow resistance based on severity. Although increased pulmonary capillary resistance is related to alveolar membrane destruction, and therefore associated with emphysema, the engine uses either emphysema severity or chronic bronchitis severity (whichever is higher) to determine the pulmonary capillary resistance multiplier. This was done in an attempt to model increased blood pressure and elevated heart rate, which are symptoms of both emphysema and chronic bronchitis. Increasing the capillary resistance should increase arterial blood pressure as the heart pumps harder to overcome the increased resistance in the lungs. 

Decreased Inspiration-Expiration (IE) ratio is another pathophysiologic feature of COPD.  As with asthma, the normal IE ratio is scaled using a multiplier based on severity. Either chronic bronchitis severity or emphysema severity (whichever is higher) is used to determine the IE ratio scaling multiplier. 

#### Acute Respiratory Distress Syndrome

Acute %Respiratory Distress Syndrome (ARDS) is modeled in the engine as a generic impairment of the alveoli's ability to exchange oxygen and carbon dioxide.  The specific cause (i.e., sepsis, pneumonia, etc.) is not specified.  The model is implemented to meet the PaO2/FiO2 Berlin Criteria combined with accepted pulmonary shunt fractions when mechanically ventilated.  A severity value is mapped to various model parameters to achieve mild, moderate, and severe symptoms at severities of 0.3, 0.6, and 0.9 respectively.  Direct modifiers are implemented as shown in @tableref {sys_validation}.

#### Pneumonia

Lobar pneumonia is a form of pneumonia that affects one or more lobes of the lungs.  Symptoms typically include increased respiration rate, decreased  tidal volume, reduced oxygen saturation, decreased IE ratio, and increased body temperature @cite ebell2006outpatient .  As fluid fills portions of the lung, it becomes more difficult to breathe. Fluid also reduces the effective gas diffusion surface area in the alveoli, reducing alveolar transfer of oxygen and carbon dioxide into and out of the bloodstream @cite guyton2006medical . The engine simulates lobar pneumonia by decreasing the alveoli compliance in the respiratory circuit, which models increased breathing difficulty due to fluid congestion in the alveoli. Similarly, gas diffusion surface area is reduced using the same function as for COPD. Decreased IE ratio is pathophysiologic feature of lobar pneumonia.  Like COPD, the normal IE ratio is scaled using a multiplier based on severity.

#### Pulmonary Fibrosis

Pulmonary Fibrosis is a restrictive lung disease characterized by alveoli wall thickening. Symptoms typically include increased respiration rate, decreased tidal volume, decreased respiratory flow, reduced oxygen saturation, decreased IE ratio, and increased body temperature. As a result, the compliance of the lung is reduced making it stiffer and more difficult to expand during inspiration @cite koeppen2002netter. The engine models increased work of breathing due to alveoli wall stiffness in pulmonary fibrosis by decreasing alveoli compliance in the respiratory circuit. To model pulmonary capillary destruction, the engine increases the pulmonary capillary flow resistance based on severity. Increasing the capillary resistance should increase arterial blood pressure as the heart pumps harder to overcome the increased resistance in the lungs. The gas diffusion area is reduced using the same function as  COPD. Decreased IE ratio is pathophysiologic feature of pulmonary fibrosis. Like COPD, the normal IE ratio is scaled using a multiplier based on severity.

#### Impaired Alveolar Exchange

The impaired alveolar exchange generically models an unspecified reduction of effective alveolar surface area.  This condition causes less effective gas exchange between the %Respiratory and %Cardiovascular systems.  The user can specify either a fraction or area value of the surface area to remove.

@anchor respiratory-actions
### Actions

#### Airway Obstruction

Airway obstruction refers to a blockage in the airway that partially or wholly
prevents airflow into or out of the lungs. Airway obstructions
can occure at the lower and upper airways. They can
be caused by foreign objects, allergic reactions, infections, inflammations,
toxic gases, and other reasons. The pathological symptoms and physiological
manifestations of airway obstructions are as diverse as the causes, and the severities
of the obstructions follow suit. Dyspnea (breathing difficulty) is the
obvious common symptom of airway obstruction. The complications of breathing
difficulty include hypoxia (low oxygen levels) and hypercapnia (high carbon
dioxide levels), respiratory acidosis, and others.

The current version of the %Respiratory Model attempts to reproduce
the physiological responses arising from foreign body upper airway obstruction.
Foreign body airway obstruction is fairly common during anesthesia. One example
is the case reported by @cite Roy2005airwayObstruction where the oxygen
saturation (SPO<SUB>2</SUB>) of a patient dropped to 40 percent due to a blockage by a
parasitic nematode during the use of a laryngeal mask airway. The %Respiratory
Model simulates airway obstruction by increasing the flow resistance across the
path connecting the airway node to the carina node by mapping a user defined severity with an exponential growth function. This path corresponds to the
flow through the trachea. The model then calculates the physiological responses
due to increased airway resistance.

#### Bronchoconstriction

A patient with bronchospasm experiences a sudden constriction of the muscles in
the walls of the bronchioles, decreasing the airway diameter. Such decrease in
the bronchial airway diameter, or bronchoconstriction, results in the reduction of
gas flow into or out of the lungs and causes difficulty in both spontaneous and
mechanical ventilation. A number of factors can trigger bronchospasms, including
a foreign body in the airway and stimulation of an endotracheal tube in patients
with reactive disease @cite Woodworth2012anesthesia . The 
%Respiratory Model accounts for this respiratory distress by adjusting the
resistance of the bronchi units by mapping a user defined severity with an exponential growth function. The increase in bronchial airway resistance results in a
decrease in gas flow in and out of the alveoli, which in turn affects the gas
concentration in the circulatory system. The engine then responds to the
respiratory distress in proportion to the level of bronchoconstriction.

#### Intubation

The Intubation action sets the type of intubation present: off, tracheal, esophageal, right mainstem, or left mainstem. Tracheal intubation is the successful state. See @ref AnesthesiaMachineMethodology for more details about intubation and mechanical ventilation.

##### Mainstem Intubation

During endotracheal intubation, a flexible tube is inserted orally or nasally
down into the trachea to facilitate ventilation of the lungs during mechanical
ventilation. A potential complication of endotracheal intubation is mainstem
intubation, an incident where the tube is inadvertently placed too deep into one
of the two mainstem bronchi. In such incidents, the unintubated lung does not
contribute to gas exchange, and a right-to-left shunt can occur, resulting in
hypoxia @cite divatia2005complications . Furthermore, the intubated lung can
be hyperinflated, predisposing to overdistension and barotrauma @cite divatia2005complications .

Right mainstem intubation is the common complication of endotracheal intubation
because of the shallower angle that the right mainstem bronchus makes with the
trachea, though the engine includes the ability to simulate left mainstem intubation as well. The %Respiratory Model attempts to simulate this incidence by
manipulating the resistance representing the opposite bronchus functional unit of
the %Respiratory System. A numerically large resistance is selected for this
circuit element for complete blockage of flow into the lung. The model then
evaluates the physiological responses arising from the incidence.

##### Esophageal Intubation

Another complication of endotracheal intubation is esophageal
intubation, an incident where the endotracheal tube is inadvertently placed in
the esophagus. When such an incidence occurs, the lung does not receive air. The
%Respiratory System models esophageal intubation by opening
the airflow path leading to the esophagus and closing the flow path leading to
the trachea compartment. This is accomplished in the model by assigning
a numerically large value for the tracheal resistance and assigning a small
resistance to the esophageal compartment. The lack of airflow into the lungs
affects the amount of alveolar-capillary gas exchange, which in turn affects the
hemodynamic and respiratory responses of the engine.

#### Acute Asthma

Asthma is a common inflammatory disease of the airways (bronchi and bronchioles) where air flow into the lungs is partially obstructed. Acute asthma is an exacerbation of asthma that does not respond to standard treatments. Symptoms include elevated respiration rate, labored breathing, and a reduction in oxygen saturation, among others.  It is generally considered a life-threatening obstruction of the airway requiring immediate medical attention. During an acute asthma attack, airflow is partially obstructed during exhalation, and flow resistance is 5 to 15 times normal @cite papiris2002asthma . The engine simulates this by increasing the airway flow resistance in the circuit model. The function used to determine the airway flow resistance multiplier is shown in the figure below. The asthma attack severity is governed by a specified severity value. The higher the severity, the more severe the asthma attack, and the higher the resistance values are set.

Additionally, the inspiratory/expiratory (IE) ratio decreases during an acute asthma attack. It takes a noticeably longer time to exhale than inhale @cite papiris2002asthma .  In the engine, the normal IE ratio is scaled using a multiplier based on severity to model decreased IE ratio during an acute asthma attack.

#### Tension Pneumothorax

Pneumothorax is an abnormal accumulation of air in the pleural cavity, i.e., a
thin space between the visceral and parietal pleura separating lungs from the
chest wall, due to factors that cause air to escape into the pleural space. The
accumulation of air in the pleural space limits the space available for full lung
inflation and increases the intrathoracic pressure, causing a collapse in the ipsilateral lung depending on the type and
severity of the incidence. Pneumothoraces are classified as traumatic and
nontraumatic or spontaneous based on their cause. Pneuomothoraces are 
classified as open (sucking chest wound) or closed (intact thoracic cage)
pnumothoraces @cite sharma2008principles .

The %Respiratory Model is capable of simulating traumatic tension
pneumothorax. Tension pneumothorax is a life-threatening condition that occurs
when air escapes into the pleural space on inspiration, but cannot return on
expiration due to a one-way valve effect of the injury. This causes a
progressive build-up of air within the pleural space. The %Respiratory
System implements a model for tension pneumothorax by including subordinate
circuit elements that represent the flow and trapping of air in the pleural
space. The circuit elements represent both the open and closed pneumothorax
condition. In the open tension pneumothorax condition, air is allowed to enter
from the atmosphere into the pleural space as in penetrating chest wound
traumas. In the closed tension pneumothorax case, the model allows air to escape
from the lung into the plural space as in laceration of the lung following blunt
trauma. The model evaluates the physiological responses arising from increased
intrapleural pressure and leakage in the airflow of the respiratory circuit.

@htmlonly
<center>
<a href="./Images/Respiratory/PneumoCirucit.png">
<img src="./Images/Respiratory/PneumoCirucit.png" width="550"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {PneumoCirucit}. Both lungs in the engine have elements to mimic the effects of open and closed tension pneumothorax insults as well as chest occlusive dressing (for open) and needle decompression (for both) interventions. The red boxes denote these additional elements.</i>
</center><br>

When a lung collapses (as with pneumothorax), increased pleural cavity pressure pushes on the mediastinum and great veins. As an effect, the mediastinum is displaced and the great veins become kinked, leading to decreased venous return to the heart. This leads to increasing cardiac and respiratory embarrasment @cite jain2008understanding . The %Cardiovascular model includes a model that maps the pleural cavity pressure difference between the right and left lungs to an increased venous return resistance.  This causes the blood pressure to decrease when a pneumothorax is present.

#### Occlusive Dressing

The management of an open tension pneumothorax requires sealing the open chest wound with an occlusive dressing. Such intervention slows the progression of tension pneumothorax and may ensure recovery if timely intervention takes place. The %Respiratory Model simulates occlusive dressing by assigning large flow resistance across the element that serves as an electrical analogue open switch for the path linking the pleural cavity to the environment. Based on this implementation, the model calculates the physiological responses arising from numerical equivalent occlusive dressing.

#### Needle Decompression

Tension pneumothorax is a life-threatening condition that requires immediate intervention to relieve the air trapped in the pleural space and reduce the intrapleural pressure to the subatmospheric level. Needle decompression is a procedure that allows air to escape from the pleural space into the environment. This procedure relieves the accumulated air, allowing full expansion of the affected lung. The %Respiratory Model is capable of simulating needle decompression by incorporating a circuit element that permits airflow based on pressure differential between the pleural space and the environment. The model then evaluates physiological responses to the intervention.

#### Hemothorax

The traumatic hemothorax model causes an accumulation of blood within the pleural cavity. The action is implemented with parts in both the %Cardiovascular and %Respiratory systems with the flow of blood out of the pulmonary veins matching effusion fluid flow into the pleural compartments. Both left and right side hemothoraces are able to be set. Either a severity or a flow rate can be set for each hemothorax action. Internal bookkeeping is used to track the total amount of blood present within the pleural cavity. Events alert when specific volumes are reached, based on the standard definition of severity classification @cite zeiler2020hemothorax. Intrapleural blood less than 400 ml is classified as a minimal hemothorax, while 400 ml to 1000 ml is a medium hemothorax. Anything greater than 1000 ml is considered a massive hemothorax. Massive hemothorax is also often defined as a bleeding rate greater than 0.2 L/hr @cite kim2020chest. Based on this definition, severity settings are mapped to flow rates with a piecewise linear function as shown in @tableref {hemothorax}.

<center><br>
*@tabledef {hemothorax}. Hemothorax severity mapping to blood flow rate. Medium hemothorax is reached at 400 mL and massive at 1000 mL.*
</center>

|	Severity	|	Flow (L/hr)	|	Time to Medium (min)	|	Time to Massive (min)	|
|	---	|	---	|	---	|	---	|
|	0.3	|	0.05	|	480	|	1200	|
|	0.6	|	0.1	|	240	|	600	|
|	0.9	|	0.2	|	120	|	300	|
|	1	|	1	|	24	|	60	|

#### Tube Thoracostomy

The chest tube thoracostomy action is used to suction the effusion caused by hemothoraces through a an optional flow rate setting. If no value is defined, a default of 200 mL/day is used, which is based on published safe drainage rates @cite hessami2009volume. A maximum flow rate of 1 mL/s is enforced to prevent model instability. The thoracostomy will also be stopped if the blood volume present within the pleural cavity is fully removed.

#### Mechanical Ventilation

Mechanical ventilation allows the user to specify an instantaneous pressure and/or flow value at the respiratory connection point (i.e., mouth).  The substance volume fractions at the connection can also be specified.  If no volume fractions are given, the ambient values set in the %Environment system will be used by default (see @ref EnvironmentMethodology).  All settings will remain constant during the simulation, unless removed or modified.  This action is likely to be most beneficial implemented in combination with real-world sensors.  Manikin or task trainer values can be fed into the the engine to synchronize in real-time.

#### Supplemental Oxygen

The supplemental oxygen action allows the user to provide a higher fraction of inspired oxygen (FiO2) to the patient by selecting one of three different oxygen therapy devices: nasal cannula, simple mask, or non-rebreather mask. The action includes optional settings for oxygen flow rate and bottle volume. If the flow is not explicitly set a default value is applied: nasal cannula = 3.5 L/min, simple mask = 7.5 L/min, non-rebreather mask = 10.0 L/min. The oxygen tank volume is set to a standard value of 425 L, if one is not set. The volume of oxygen is the value that can be provided when depressurized leaving the tank, often referred to as "capacity." The supplemental oxygen action decrements the volume able to be delivered that is remaining in the tank every time-step during simulation. The volume remaining in the tank can be updated by the user anytime while the action is applied.

Each of the three supplemental oxygen devices is modeled using a different lumped parameter circuit that connects directly with the respiratory system - see @figureref {SupplementalOxygenCirucits}. The nasal cannula and simple mask both have a high pressure source with a high resistance to provide the requested flow of oxygen gas.  Replacing this section of the circuit with a flow source proved to be problematic due to ideal circuit laws that dictate the pressure across a current source can be any value. Since the non-rebreather mask uses a very high compliance bag that is continuously filled with oxygen, a pressure source is not needed. The volume of the bag is updated every time-step based on the flow in from the tank and out from inhalation. There is a maximum bag volume of 1 L. A volume below 0 L stops oxygen from flowing out of the bag.

The seal resistance in each circuit dictates how much air escapes due to be secured imperfectly. The nasal cannula has a low seal value to realistically represent the prongs in the patient's nostrils.

@htmlonly
<center>
<a href="./Images/Respiratory/SupplementalOxygenCirucits.png">
<img src="./Images/Respiratory/SupplementalOxygenCirucits.png" width="1000"></a>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {SupplementalOxygenCirucits}. Each supplemental oxygen device has a different circuit to mimic the air mixing nuances of each.</i>
</center><br>

#### Conscious Respiration 

Conscious respiration consists of a set of commands that model forced exhalation, forced inhalation, holding breath, and inhaler actuation. Collectively, they allow the engine to model coordinated use of an inhaler.  It should be noted that the conscious respiration action begins immediately when called, and will continue until completed while the simulation continues.  Users will likely want to advance time for the duration of the conscious respiration command before attempting other actions or completing a scenario.  The following commands can be used in any order and will wait until the completion of the previous command to begin:

- <b>Forced Exhale</b>: The "depth" or "shallowness" of the exhalation is determined by the expiratory reserve volume fraction, which specifies what portion of the expiratory reserve volume to exhale. If set to 1.0, the patient exhales his or her entire expiratory reserve volume, leaving only the residual volume in the lungs. The time period over which to execute the exhale is determined by the period. 
- <b>Forced Inhale</b>: The "depth" or "shallowness" of the inhalation is determined by the inspiratory capacity fraction, which specifies what portion of the inspiratory capacity to inhale. If set to 1.0, the patient inhales his or her entire inspiratory capacity, completely filling the lungs. The time period over which to execute the inhale is determined by the period. 
- <b>Forced Pause</b>: The time period to hold breath at functional residual capacity (0 drive pressure) is determined by the period. 
- <b>Use %Inhaler</b>: The %Inhaler command is interpreted by the inhaler equipment (@ref InhalerMethodology).

Conscious respiration has any number of potential applications and is likely to be implemented to attain proper breathing while using an inhaler, generate a spirometry curve, or simulate coughing.  @figureref {Experimental_Cough_Flow} shows the results for a cough scenario that leverages the conscious respiration action compared to empirical data.

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/Engine_Cough_Flow.jpg"><img src="./plots/Respiratory/Engine_Cough_Flow.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/Experimental_Cough_Flow.jpg"><img src="./plots/Respiratory/Experimental_Cough_Flow.jpg" width="550"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {Experimental_Cough_Flow}. The airflow curve of a simulated cough in engine generated from a conscious respiration with quick forced inhale and exhale in series. For comparison, the figure includes a plot reproduced from 
literature determined by a voluntary cough immediately following office-based vocal fold medialization injections @cite ruddy2014improved.</i>
</center><br>

#### Exacerbations

Several respiratory conditions have exacerbation actions defined to allow for increased/decreased severities during runtime, including COPD, ARDS, and Lobar Pneumonia.  These exacerbation actions will instantaneously (i.e., during the simulation/scenario runtime) change the values shown in @tableref {sys_validation}, based on the severity provided.  Exacerbations can either degrade or improve the patient's current condition.

@anchor respiratory-events
Events
------

### Maximum Pulmonary Ventilation Rate

The %Respiratory system triggers Maximum Pulmonary Ventilation Rate event to alert the user when the patient pulmonary ventilation exceeds maximum value.

### Bradypnea

Bradypnea is defined as an abnormally low breathing rate. Normal breathing rates for healthy adults are relatively standard, but can vary based upon condition. Low breathing rates can decrease total ventilation if not compensated for, which can lead to hypercapnia or hypoxia. In the engine, bradypnea occurs when the respiratory rate falls below 10 breaths per minute @cite overdyk2007continuous and returns to normal when it rises back above 10.5 breaths per minute.

### Tachypnea

Tachypnea is defined as high breathing rate. Normal respiration rate in an adult ranges 12 - 20 breaths per minute. Tachypnea occurs when breathing rate rises above 20 breaths per minute. The engine outputs a tachypnea event when the patient's respiration rate is above 20 breaths per minute. 

### Respiratory Acidosis

%Respiratory acidosis is triggered when the blood pH drops below 7.37.  An irreversible state (similar to a death state) is reached when the blood pH drops below 6.5.  See @ref EnergyMethodology for more details about acidosis.

### Respiratory Alkalosis

%Respiratory alkalosis is triggered when the blood pH rises above 7.45.  An irreversible state (similar to a death state) is reached when the blood pH rises above 8.5.  See @ref EnergyMethodology for more details about alkalosis.

@anchor respiratory-results
Results and Conclusions
=======================

The %Respiratory System was validated quantitatively and qualitatively under resting physiological conditions and transient (action-induced) conditions. The validation is specified with a color coding system, with green indicating good agreement with trends/values, yellow indicating moderate agreement with trends/values, and red indicating poor agreement with trends/values.

@anchor respiratory-validation
Validation - Resting Physiologic State
--------------------------------------

The %Respiratory Model outputs a number of system-level and compartment-level resting physiologic parameters. The tables below compare the values of system- and compartment-level parameters obtained from the model with referenced values. The outputs from the model correspond to the system- and compartment-level respiratory related values of the standard male and female patients under resting physiologic conditions. As shown in @tableref {sys_validation}, the majority of the physiological variables show a good match with the literature. The outputs for the major respiratory variables, such as respiration rate, tidal volume, and pulmonary ventilation specifically match well with the reference data. 

<center>
<i>@tabledef {sys_validation}. Validation of the resting physiologic state of the %Respiratory System. The table shows comparison of system-level outputs from the engine to referenced values. System-level outputs show favorable agreement with validation data. The deviations in end tidal carbon dioxide fraction and transpulmonary pressure can be attributed to the tuned parameters used in the model that are selected to meet the major system level physiological parameters.</i>
</center><br>

<b>Standard Male</b>

@insert ./test_results/tables/Respiratory-StandardMaleValidationTable.md

<b>Standard Female</b>

@insert ./test_results/tables/Respiratory-StandardFemaleValidationTable.md

<center>
<i>@tabledef {patient_validation}. Validation table for the resting physiologic states. The table shows comparison of compartment-level outputs from the engine to referenced values. The majority of the variables show good match with the validation data. There are significant deviations in the carina flow and the dead space volumes. Future versions will address these issues.</i>
</center><br>

<b>Standard Male</b>

@insert ./test_results/tables/RespiratoryCompartments-StandardMaleValidationTable.md

<b>Standard Female</b>

@insert ./test_results/tables/RespiratoryCompartments-StandardFemaleValidationTable.md

@anchor respiratory-scenario
Scenario Validation
-------------------

The actions and interventions associated with the %Respiratory System were validated quantitatively where possible and qualitatively elsewhere by comparing the engine output to expected trends and values. For each scenario, the table shows the total number of results in each category. For many investigated scenarios, the model shows good agreement with the expected trends. For the scenarios that did not match with the expected trends, improvements are planned for future engine releases.

<center><br>
*@tabledef {cond_validation}. Cumulative validation results for %Respiratory specific conditions and actions scenarios.*
</center>

|	Key	|
|	---	|
|<span class="success">	Good agreement: correct trends or <10% deviation from expected	</span>|
|<span class="warning"> 	Some deviation: correct trend and/or <30% deviation from expected	</span>|
|<span class="danger">	Poor agreement: incorrect trends or >30% deviation from expected	</span>|

|	Scenario 	|	Description	|	Good	|	Decent	|	Bad	|
|	---	|	---	|	---	|	---	|	---	|
|	AsthmaAttackModerateAcute	|	Moderate acute asthma attack	|<span class="success">	18	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	AsthmaAttackSevereAcute	|	Severe acute asthma attack	|<span class="success">	13	</span>|<span class="warning">	4	</span>|<span class="danger">	1	</span>|
|	AsthmaAttackLifeThreateningAcute	|	Life threatening acute asthma attack	|<span class="success">	16	</span>|<span class="warning">	1	</span>|<span class="danger">	1	</span>|
|	COPDSevereEmphysema	|	GOLD Stage III Emphysema	|<span class="success">	5	</span>|<span class="warning">	3	</span>|<span class="danger">	1	</span>|
|	COPDSevereBronchitisLeft	|	Severe Chronic Bronchitis	|<span class="success">	6	</span>|<span class="warning">	2	</span>|<span class="danger">	1	</span>|
|	ARDSModerateBothLungs	|	Moderate Acute Respiratory Distress Syndrome	|<span class="success">	9	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	PneumoniaSevereLeftLobe	|	Severe Lobar Pneumonia in one lobe in the left lung	|<span class="success">	1	</span>|<span class="warning">	5	</span>|<span class="danger">	3	</span>|
|	PneumoniaSevereRightLung	|	Severe Lobar Pneumonia in two lobes of right lung	|<span class="success">	5	</span>|<span class="warning">	2	</span>|<span class="danger">	2	</span>|
|	PneumoniaModerateBothLungs	|	Moderate Lobar Pneumonia in both lungs	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	2	</span>|
|	PulmonaryFibrosisSevere	|	Severe Pulmonary Fibrosis	|<span class="success">	7	</span>|<span class="warning">	0	</span>|<span class="danger">	2	</span>|
|	TensionPneumothoraxOpenVaried	|	Varied open pneumothorax severities and interventions	|<span class="success">	40	</span>|<span class="warning">	1	</span>|<span class="danger">	1	</span>|
|	TensionPneumothoraxClosedVaried	|	Varied closed pneumothorax severities and interventions	|<span class="success">	32	</span>|<span class="warning">	1	</span>|<span class="danger">	2	</span>|
|	HemothoraxVaried	|	Varied hemothorax rates and interventions	|<span class="success">	39	</span>|<span class="warning">	3	</span>|<span class="danger">	0	</span>|
|	AirwayObstructionVaried	|	Airway Obstruction with varying severities	|<span class="success">	26	</span>|<span class="warning">	0	</span>|<span class="danger">	4	</span>|
|	Bronchoconstriction	|	Bronchoconstriction with varying severities	|<span class="success">	24	</span>|<span class="warning">	0	</span>|<span class="danger">	6	</span>|
|	MainstemIntubation	|	Right and left mainstem intubation and correction (with Succs)	|<span class="success">	20	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	EsophagealIntubation	|	Esophageal intubation and correction (with Succs)	|<span class="success">	10	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Dyspnea	|	Varied severities of respiratory apnea	|<span class="success">	11	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|	Supplemental Oxygen	|	Nasal cannula, simple mask, and non-rebreather mask	|<span class="success">	3	</span>|<span class="warning">	0	</span>|<span class="danger">	0	</span>|
|		|	Total	|<span class="success">	292	</span>|<span class="warning">	22	</span>|<span class="danger">	26	</span>|

@anchor respiratory-conditionvalidation
Validation - Conditions
-----------------------

### COPD

The COPD condition was validated against two scenarios. The severe emphysema scenario (Emphysema Severity = 0.7, Bronchitis Severity = 0.5) attempts to model Gold Stage III Emphysema.

<center><br>
<i>@tabledef {emphysema}. Validation matrix for severe emphysema. This condition corresponds to GOLD Stage III COPD  @cite man2003contemporary  @cite perez2009can . The table shows the engine output compared to validation data for respiratory and hemodynamic values.</i>
</center>

|	Segment	|	Notes	|	Sampled Scenario Time (s)	|	Trachea Flow - Peak Expiratory Flow  (L/min)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Severe Emphysema: Bronchitis Severity = 0.5 Emphysema Severity = 0.7 Both Lungs	|	GOLD Stage III	|	120	|<span class="success">	< 70% of Normal @cite perez2009can	</span>|<span class="warning">	Increase,  Tachypnea, Dyspnea,  > 30 @cite gunning2003pathophysiology	</span>|<span class="warning">	< 60% of normal @cite gunning2003pathophysiology	</span>|<span class="success">	Increase, Tachycardia	</span>|<span class="danger">	Increase,   Pulmonary Hypertension ,  > 140 mm Hg @cite keller2003pathophysiology, @cite scharf2002hemodynamic	</span>|<span class="success">	< 90%  @cite man2003contemporary; < 89% @cite keller2003pathophysiology	</span>|<span class="success">	Decrease,  Hypoxemia @cite keller2003pathophysiology; < 55 mm Hg  @cite man2003contemporary, @cite keller2003pathophysiology	</span>|<span class="warning">	Increase, Hypercapnia, > 55 mmHg @cite keller2003pathophysiology	</span>|<span class="success">	Decrease @cite van1991physical	</span>|

<center><br>
<i>@tabledef {bronchitis}. Validation matrix for severe chronic bronchitis. The table shows the engine output compared to validation data for respiratory and hemodynamic values.</i>
</center>

|	Segment	|	Notes	|	Sampled Scenario Time (s)	|	Trachea Flow - Peak Expiratory Flow  (L/min)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Severe Chronic Bronchitis: Bronchitis Severity = 0.7 Emphysema Severity = 0.5 Both Lungs	|		|	120	|<span class="success">	Decrease @cite bergeronSME	</span>|<span class="success">	> 20 @cite bergeronSME	</span>|<span class="warning">	< 60% of normal @cite bergeronSME	</span>|<span class="success">	Increase,  Tachycardia @cite bergeronSME	</span>|<span class="danger">	Increase,  Pulmonary Hypertension,  > 140 mm Hg @cite bergeronSME	</span>|<span class="success">	< 90% @cite bergeronSME	</span>|<span class="success">	Decrease,  Hypoxemia,  < 55 mm Hg  @cite bergeronSME	</span>|<span class="warning">	Increase,   Hypercapnia > 55 mmHg   @cite bergeronSME	</span>|<span class="success">	Decrease @cite van1991physical, @cite bergeronSME	</span>|

### Acute Respiratory Distress Syndrome

The ARDS condition was validated against a moderate severity scenario.  Further validation of ARDS while ventilated can be found in the @ref MechanicalVentilatorMethodology validation.

<center><br>
<i>@tabledef {ards}. Validation matrix for moderate ARDS in both lungs. The table shows the engine output compared to validation data for respiratory and hemodynamic values.</i>
</center>

|	Segment	|	Notes	|	Sampled Scenario Time (s)	|	Pulmonary Compliance (L/cmH2O)	|	Respiration Rate (breaths/min)	|	Shunt Fraction	|	Heart Rate (beats/min)	|	Total Pulmonary Ventilation (L/min) 	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	Horowitz Index [PaO2/FiO2] (mmHg)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Moderate ARDS Severity = 0.6 Left Lung = 100% Right Lung  = 100%	|	Chronic condition	|	120	|<span class="success">	Reduced @cite mortelliti2002acute	</span>|<span class="success">	Tachypnea @cite mortelliti2002acute	</span>|<span class="success">	>20% @cite radermacher2017fifty	</span>|<span class="success">	Tachycardia @cite mortelliti2002acute	</span>|<span class="success">	Increased @ cite mortelliti2002acute	</span>|<span class="success">	Hypoxia @cite mortelliti2002acute	</span>|<span class="success">	Hypoxemia @cite radermacher2017fifty	</span>|<span class="success">	Normal or near normal @cite mortelliti2002acute	</span>|<span class="success">	<300 @cite mortelliti2002acute	</span>|

### Pneumonia

The pneumonia condition was validated against three scenarios; severe pneumonia in the one lobe of the left lung (Severity = 0.7), severe pneumonia in two lobes of the right lung (Severity = 0.7), and moderate pneumonia in both lungs (Severity = 0.2). The engine does not model the discrete lobes of each lung, so infected-lobe behavior is modeled by applying pneumonia severity against a fraction of each lung. The fraction approximates the portion of the lung (by volume) that the lobe(s) occupy. In the case of the left lung, there are two lobes. Each lobe constitutes approximately 50% of the whole lung. It is understood that the actual volume distribution is not equal for the two lobes, but we use this as an approximation. The same is true for the right lung, which has three lobes. Increasing core body temperature is also a symptom of lobar pneumonia that fails to agree with the validation data. However, we expected this disagreement since the engine doesn't currently model elevated body temperature in response to infection. This is something we plan to address in future releases. 

<center><br>
<i>@tabledef {sev_pneumonia1}. Validation matrix for severe lobar pneumonia concentrated in one lobe in the left lung. The table shows the engine output compared to validation data for respiratory and hemodynamic values.</i>
</center>

|	Condition	|	Notes	|	Sampled Scenario Time (s)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|	Core Body Temperature  (Degrees C)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Pneumonia:  Left Lung Severity = 0.45 Right Lung Severity = 0	|	Severe Lobar Pneumonia in one lobe in the left lung. Current model does not include metabolic effects.	|	120	</span>|<span class="warning">	Increase,  Tachypnea, Dyspnea  > 20 @cite ebell2007predicting	</span>|<span class="success">	Decrease @cite bergeronSME	</span>|<span class="warning">	Increase,  Tachycardia, > 100 @cite ebell2007predicting	</span>|<span class="warning">	Decrease, < 90 mm Hg @cite fine1997prediction	</span>|<span class="warning">	Decrease , < 95% @cite majumdar2010oxygen	</span>|<span class="warning">	Decrease,  Hypoxemia @cite fine1997prediction < 60 mm Hg	</span>|<span class="danger">	Increase @cite bergeronSME	</span>|<span class="danger">	Decrease @cite bergeronSME	</span>|<span class="danger">	Increase,   > 37.8 C (100 F) @cite ebell2007predicting	</span>|

<center><br>
<i>@tabledef {sev_pneumonia2}. Validation matrix for severe lobar pneumonia concentrated in two lobes in the right lung. The table shows the engine output compared to validation data for respiratory and hemodynamic values.</i>
</center>

|	Condition	|	Notes	|	Sampled Scenario Time (s)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|	Core Body Temperature  (Degrees C)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Pneumonia:  Left Lung Severity = 0 Right Lung Severity = 0.9	|	Severe lobar pneumonia in two lobes of the  right lung. Current model does not include metabolic effects.	|	120	</span>|<span class="warning">	Increase,  Tachypnea, Dypsnea  > 20 @cite ebell2007predicting	</span>|<span class="success">	Decrease @cite bergeronSME	</span>|<span class="success">	Increase,  Tachycardia, > 100 @cite ebell2007predicting	</span>|<span class="success">	Decrease, < 90 mm Hg @cite fine1997prediction	</span>|<span class="success">	Decrease , < 95% @cite majumdar2010oxygen	</span>|<span class="success">	Decrease,  Hypoxemia @cite fine1997prediction < 60 mm Hg	</span>|<span class="warning">	Increase @cite bergeronSME	</span>|<span class="danger">	Decrease @cite bergeronSME	</span>|<span class="danger">	Increase,   > 37.8 C (100 F) @cite ebell2007predicting	</span>|

<center><br>
<i>@tabledef {mod_pneumonia2}. Validation matrix for moderate lobar pneumonia in both lungs. The table shows the engine output compared to validation data for respiratory and hemodynamic values.</i>
</center>

|	Condition	|	Notes	|	Sampled Scenario Time (s)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|	Core Body Temperature  (Degrees C)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Pneumonia:  Left Lung Severity = 0.6 Right Lung Severity = 0.6	|	Moderate pneumonia in both lungs. Current model does not include metabolic effects.	|	120	|<span class="success">	Increase,  Tachypnea, Dypsnea  > 20 @cite ebell2007predicting	</span>|<span class="success">	Decrease @cite bergeronSME	</span>|<span class="success">	Increase,  Tachycardia, > 100 @cite ebell2007predicting	</span>|<span class="success">	Decrease, < 90 mm Hg @cite fine1997prediction	</span>|<span class="success">	Decrease , < 95% @cite majumdar2010oxygen	</span>|<span class="success">	Decrease,  Hypoxemia @cite fine1997prediction < 60 mm Hg	</span>|<span class="success">	Increase @cite bergeronSME	</span>|<span class="danger">	Decrease @cite bergeronSME	</span>|<span class="danger">	Increase,   > 37.8 C (100 F) @cite ebell2007predicting	</span>|

### Pulmonary Fibrosis

The pulmonary fibrosis condition was validated with a severe pulmonary fibrosis scenario by applying a severity of 0.9 to the patient.The severity decreased the lung compliance and increased the airway resistance making it difficult to breathe.The resulting outputs are shown in Table 11 for common clinical parameters.

<center><br>
<i>@tabledef {fibrosis}. Validation matrix for severe pulmonary fibrosis. The table shows the engine output compared to validation data for respiratory and hemodynamic values.</i>
</center>

|	Segment	|	Notes	|	Sampled Scenario Time (s)	|	Trachea Flow - Peak Expiratory Flow  (L/min)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Severe Pulmonary Fibrosis	|	Severity = 0.9	|	120	|<span class="success">	Decreases	</span>|<span class="success">	Increase,  Tachypnea,   > 30 @cite gunning2003pathophysiology	</span>|<span class="success">	Dyspnea, < 60% of normal @cite nava1999lung, @cite gunning2003pathophysiology	</span>|<span class="success">	Increase, Tachycardia	</span>|<span class="danger">	Increase, Pulmonary Hypertension, > 140 mm Hg @cite raghu2015pulmonary	</span>|<span class="success">	< 90%  	</span>|<span class="success">	Decrease,  Hypoxemia @cite costanzo2010physiology	</span>|<span class="success">	Increase, Hypercapnia	</span>|<span class="danger">	Decrease 	</span>|

@anchor respiratory-actionvalidation
Validation - Actions
--------------------

@insert ./validation/markdown/AirwayObstruction.md

### Bronchoconstriction

The bronchoconstriction action was validated with a scenario that applies varying severities of 0.3, 0.6, and 1.0 to the patient. Varying the severity of the obstruction simulates the constriction of the bronchii, leading to obstructed air flow. The severity levels logarithmically increase the airway resistance.

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/BronchoConstrictionVaried_TidalVolume.jpg"><img src="./plots/Respiratory/BronchoConstrictionVaried_TidalVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/BronchoConstrictionVaried_CO2PP.jpg"><img src="./plots/Respiratory/BronchoConstrictionVaried_CO2PP.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/BronchoConstrictionVaried_O2Sat.jpg"><img src="./plots/Respiratory/BronchoConstrictionVaried_O2Sat.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/BronchoConstrictionVaried_MAP.jpg"><img src="./plots/Respiratory/BronchoConstrictionVaried_MAP.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><a href="./plots/Respiratory/BronchoConstrictionVariedLegend.jpg"><img src="./plots/Respiratory/BronchoConstrictionVariedLegend.jpg" width="1100"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {BronchoConstrictionVaried}. Select outputs from the bronchoconstriction scenario.</i>
</center><br>

<center>
<i>@tabledef {broncho}. Validation matrix for physiological responses due to varying severities of bronchoconstriction.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time  (s)	|	Sampled Scenario Time  (s)	|	Respiration Rate (breaths/min)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Diastolic Pressure (mmHg)	|	Oxygen Saturation	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Trigger Bronchoconstriction; severity 0.3	|	Trigger bronchoconstriction of severity 0.3 by Increasing effective breathing resistances on bronchi. Not severe enough to trigger CV responses.	|	30	|	180	|<span class="danger">	Decrease  @cite Morgan2006Clinical	</span>|<span class="danger">	Increase  tachycardia  @cite faust2002anesthesiology, @cite Morgan2006Clinical	</span>|<span class="danger">	Decrease hypotension  @cite faust2002anesthesiology	</span>|<span class="danger">	Decrease hypotension  @cite faust2002anesthesiology	</span>|<span class="success">	Decrease  @cite Morgan2006Clinical	</span>|
|	Remove bronchoconstriction	|	Bronchoconstriction is turned off.	|	210	|	300	|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|
|	Trigger Bronchoconstriction;   severity = 0.6	|	Trigger bronchoconstriction of severity 0.6 by Increasing effective breathing resistances on bronchi.	|	510	|	180	|<span class="success">	Increase > Severity 0.3  @cite Morgan2006Clinical	</span>|<span class="success">	Increase  tachycardia  @cite faust2002anesthesiology, @cite Morgan2006Clinical	</span>|<span class="success">	Decrease hypotension  @cite faust2002anesthesiology	</span>|<span class="danger">	Decrease hypotension  @cite faust2002anesthesiology	</span>|<span class="success">	Decrease < Severity 0.3   @cite Morgan2006Clinical	</span>|
|	Remove bronchoconstriction	|	Bronchoconstriction is turned off.	|	690	|	300	|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|
|	Trigger Bronchoconstriction;   severity = 1.0	|	Trigger bronchoconstriction of severity 1.0 by Increasing effective breathing resistances on bronchi.	|	990	|	180	|<span class="success">	0, Dyspnea	</span>|<span class="success">	Increase  tachycardia  @cite faust2002anesthesiology, @cite Morgan2006Clinical	</span>|<span class="success">	Decrease hypotension  @cite faust2002anesthesiology	</span>|<span class="danger">	Decrease hypotension  @cite faust2002anesthesiology	</span>|<span class="success">	Decrease < Severity 0.6   @cite Morgan2006Clinical	</span>|
|	Remove bronchoconstriction	|	Bronchoconstriction is turned off.	|	1170	|	500	|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|<span class="success">	returns to resting physiology 	</span>|

### Pneumothorax

Two scenarios were used to validate tension pneuomthorax and its associated interventions, occlusive dressing, and needle decompression. @tableref {pneumothorax_open} demonstrates the open tension pneumothorax on the left side of the chest at increasing severities treated with an occlusive dressing and needle decompression. The engine output for relevant parameters was compared to the validation data. The blood pressures (systolic, diastolic, and mean arterial pressure) shows hypotension during the open wound pneumothorax incidences on a single lung due to the mediastinum shift. Other respiratory and cardiovascular outputs show good agreement with published data. Vital signs become more dire the longer the pneumothorax is applied without intervention and as the severity (i.e. hole size) is increased. Evidence suggests that patients have progressive respiratory deterioration with final respiratory arrest @cite leigh2005tension. However, the current model does not include a definition for respiratory arrest and will continue to run until another irreversible state is reached. After an occlusive dressing is added to seal the hole and needle decompression is applied, the patient's condition improves and vitals begin returning toward normal.  This is as expected compared to the validation data. 

It is important to note nervous system responses of a conscious patient due to pain or panicking are not included with the Pneumothorax action. Some of these responses can be attained by including an Acute Stress action (see @ref endocrine-actions "Endocrine Methodology").

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/TensionPneumothoraxOpenVaried_TotalLungVolume.jpg"><img src="./plots/Respiratory/TensionPneumothoraxOpenVaried_TotalLungVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/TensionPneumothoraxOpenVaried_LeftLungVolume.jpg"><img src="./plots/Respiratory/TensionPneumothoraxOpenVaried_LeftLungVolume.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/TensionPneumothoraxOpenVaried_O2Sat.jpg"><img src="./plots/Respiratory/TensionPneumothoraxOpenVaried_O2Sat.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/TensionPneumothoraxOpenVaried_MAP.jpg"><img src="./plots/Respiratory/TensionPneumothoraxOpenVaried_MAP.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><a href="./plots/Respiratory/TensionPneumothoraxOpenVariedLegend.jpg"><img src="./plots/Respiratory/TensionPneumothoraxOpenVariedLegend.jpg" width="1100"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {TensionPneumothoraxOpenVaried}. Select outputs from the open tension pneumothorax scenario.</i>
</center><br>

<center>
<i>@tabledef {pneumothorax_open}. Validation matrix for physiological responses due to varying severities of open tension pneumothorax, as well as occlusive dressing and needle decompression interventions.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Diastolic Pressure (mmHg)	|	Mean Arterial Pressure (mmHg)	|	Oxygen Saturation	|	End Tidal CO2 (mmHg)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Tension Pneumothorax;  Open, severity = 0.0	|	A severity of zero should not change the leak resistance from the default open switch value.	|	0	|	15	|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|
|	Tension Pneumothorax;  Open,  severity = 0.3	|	Represents a "sucking" chest wound on the left lung side with a mild severity (medium size hole).	|	30	|	300	|<span class="success">	 No change or Increase  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="danger">	 Increase  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Increase  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Decrease  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Decrease  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Decrease  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Decrease  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	Modest increase, then decrease as it evolves @cite sullivan2016tension	</span>|
|	Tension Pneumothorax;  Open,  severity = 0.6	|	Represents a "sucking" chest wound on the left lung side with a moderate severity (large size hole).	|	360	|	600	|<span class="success">	 Increase @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	Increase @cite leigh2005tension	</span>|<span class="success">	Increase @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease < 35 @cite sullivan2016tension	</span>|
|	Tension Pneumothorax;  Open,  severity = 1.0	|	Represents a "sucking" chest wound on the left lung side with the severe severity (maximum size hole).  Should lead to full collapse.	|	660	|	900	|<span class="success">	 Clinical sign: Tachypnea  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	Increase @cite leigh2005tension	</span>|<span class="success">	Clinical sign: Tachycardia  @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Clinical sign: Hypotension @cite bond200968w, @cite leigh2005tension	</span>|<span class="warning">	Clinical sign: Hypotension @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Clinical sign: Hypotension @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Clinical sign: Hypoxia  @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease < 35 @cite sullivan2016tension	</span>|
|	Apply Chest Occlusive dressing on the left side 	|	Represents the closing of chest wound on the left side	|	960	|	1000	|<span class="success">	No Change or Increased  @cite bergeronSME	</span>|<span class="success">	No Change 	</span>|<span class="success">	Tachycardia > 120 @cite bergeronSME	</span>|<span class="success">	Slightly improved to no change  @cite bergeronSME	</span>|<span class="success">	Slightly improved to no change  @cite bergeronSME	</span>|<span class="success">	Slightly improved to no change  @cite bergeronSME	</span>|<span class="success">	Modest decrease to no change  @cite bergeronSME	</span>|<span class="success">	No Change 	</span>|
|	Apply needle decompression	|	Needle decompression is applied on the left side of the chest	|	1020	|	1320	|<span class="success">	Returning toward normal @cite bergeronSME	</span>|<span class="success">	Returning toward normal @cite bergeronSME	</span>|<span class="success">	Returning toward normal @cite bergeronSME	</span>|<span class="success">	Slightly diminished from normal @cite bergeronSME	</span>|<span class="success">	Slightly diminished from normal @cite bergeronSME	</span>|<span class="success">	Slightly diminished from normal @cite bergeronSME	</span>|<span class="success">	Normal @cite bergeronSME	</span>|<span class="success">	Rapidly increase @cite sullivan2016tension	</span>|

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/TensionPneumothoraxClosedVaried_TotalLungVolume.jpg"><img src="./plots/Respiratory/TensionPneumothoraxClosedVaried_TotalLungVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/TensionPneumothoraxClosedVaried_RightLungVolume.jpg"><img src="./plots/Respiratory/TensionPneumothoraxClosedVaried_RightLungVolume.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/TensionPneumothoraxClosedVaried_O2Sat.jpg"><img src="./plots/Respiratory/TensionPneumothoraxClosedVaried_O2Sat.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/TensionPneumothoraxClosedVaried_MAP.jpg"><img src="./plots/Respiratory/TensionPneumothoraxClosedVaried_MAP.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><a href="./plots/Respiratory/TensionPneumothoraxClosedVariedLegend.jpg"><img src="./plots/Respiratory/TensionPneumothoraxClosedVariedLegend.jpg" width="1100"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {TensionPneumothoraxClosedVaried}. Select outputs from the closed tension pneumothorax scenario.</i>
</center><br>

<center>
<i>@tabledef {pneumothorax_closed}. Validation matrix for physiological responses due to varying severities of closed tension pneumothorax, as well as needle decompression interventions.</i>
</center>

|	Segment	|	Notes	|	Action Occurance Time (s)	|	Sampled Scenario Time (s)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Diastolic Pressure (mmHg)	|	Mean Arterial Pressure (mmHg)	|	Oxygen Saturation	|	Oxygen Saturation	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Tension Pneumothorax;  Closed,  severity = 0.0	|	A severity of zero should not change the leak resistance from the defualt open switch value.	|	0	|	15	|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|
|	Tension Pneumothorax;  Closed,   severity = 0.3	|	Represents a leak on the right lung side with a mild severity (medium size hole).	|	30	|	300	|<span class="success">	 No change or Increase  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="danger">	 Increase  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Increase  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Decrease  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Decrease  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Decrease  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	 No change or Decrease  @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	Modest increase, then decrease as it evolves @cite sullivan2016tension	</span>|
|	Tension Pneumothorax;  Closed,   severity = 0.6	|	Represents a leak on the right lung side with a moderate severity (large size hole).	|	360	|	600	|<span class="success">	 Increase @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	Increase @cite leigh2005tension	</span>|<span class="success">	Increase  @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease  @cite bond200968w, @cite leigh2005tension	</span>|<span class="danger">	Decrease @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease < 35 @cite sullivan2016tension	</span>|
|	Tension Pneumothorax;  Closed,   severity = 1.0	|	Represents a leak on the right lung side with the severe severity (maximum size hole).  Should lead to full collapse.	|	660	|	900	|<span class="success">	 Clinical sign: Tachypnea      @cite bond200968w, @cite leigh2005tension 	</span>|<span class="success">	Increase @cite leigh2005tension	</span>|<span class="success">	Clinical sign: Tachycardia   @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Clinical sign: Hypotension  @cite bond200968w, @cite leigh2005tension	</span>|<span class="warning">	Clinical sign: Hypotension  @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Clinical sign: Hypotension  @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Clinical sign: Hypoxia   @cite bond200968w, @cite leigh2005tension	</span>|<span class="success">	Decrease < 35 @cite sullivan2016tension	</span>|
|	Apply needle decompression	|	Needle decompression is applied on the right side of the chest	|	960	|	1260	|<span class="success">	Slightly lower tachypnea  @cite bergeronSME	</span>|<span class="success">	Returning toward normal @cite bergeronSME	</span>|<span class="success">	Tachycardia, but may be lower value  @cite bergeronSME	</span>|<span class="success">	Slightly diminished from normal @cite bergeronSME	</span>|<span class="success">	Slightly diminished from normal @cite bergeronSME	</span>|<span class="warning">	Slightly diminished from normal @cite bergeronSME	</span>|<span class="success">	Normal @cite bergeronSME	</span>|<span class="success">	Rapidly increase @cite sullivan2016tension	</span>|


### Hemothorax

The hemothorax and tube thoracostomy actions were validated with a scenario that incrementally increases the severity of the flow rate until a massive hemothorax is achieved. A tube thoracostomy is then applied to remove the fluid from the pleural space and alleviate the symptoms. Common clinical vital signs were compared to validation data, as shown in @tableref {hemothorax_left}.

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/HemothoraxVaried_TotalLungVolume.jpg"><img src="./plots/Respiratory/HemothoraxVaried_TotalLungVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/HemothoraxVaried_LeftLungVolume.jpg"><img src="./plots/Respiratory/HemothoraxVaried_LeftLungVolume.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/HemothoraxVaried_BloodVolume.jpg"><img src="./plots/Respiratory/HemothoraxVaried_BloodVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/HemothoraxVaried_O2Sat.jpg"><img src="./plots/Respiratory/HemothoraxVaried_O2Sat.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><a href="./plots/Respiratory/HemothoraxVariedLegend.jpg"><img src="./plots/Respiratory/HemothoraxVariedLegend.jpg" width="800"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {HemothoraxVaried}. Select outputs from the hemothorax varied scenario.</i>
</center><br>

<center>
<i>@tabledef {hemothorax_left}. Validation matrix for physiological responses due to varying flow rates of a left hemothorax and remove of fluid with a tube thoracostomy.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Blood Volume (mL)	|	Left Lung Volume (mL)	|	Respiration Rate (breaths/min)	|	Heart Rate (beats/min)	|	Mean Arterial Pressure (mmHg)	|	Pulse Pressure (mmHg)	|	Oxygen Saturation	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Hemothorax: severity = 0.0	|	A severity of zero should not change the leak resistance from the default open switch value.	|	30	|	60	|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|<span class="success">	No Change 	</span>|
|	Hemothorax: flow rate = 0.1 mL/s	|	Represents a minimal traumatic hemothorax on the left lung side.	|	60	|	780	|<span class="success">	~72 reduction from initial @cite zeiler2020hemothorax 	</span>|<span class="success">	Decrease  @cite dogrul2020blunt	</span>|<span class="success">	Increase @cite gomez2022hemothorax 	</span>|<span class="success">	Increase @cite dogrul2020blunt	</span>|<span class="warning">	Decrease @cite dogrul2020blunt	</span>|<span class="success">	Decrease @cite gomez2022hemothorax	</span>|<span class="warning">	Decrease @cite gomez2022hemothorax	</span>|
|	Hemothorax: flow rate = 0.5 mL/s	|	Represents a medium traumatic hemothorax on the left lung side.	|	780	|	1500	|<span class="success">	~432 reduction from initial @cite zeiler2020hemothorax	</span>|<span class="success">	Decrease  @cite dogrul2020blunt	</span>|<span class="success">	Increase @cite gomez2022hemothorax 	</span>|<span class="success">	Increase @cite dogrul2020blunt	</span>|<span class="warning">	Decrease @cite dogrul2020blunt	</span>|<span class="success">	Decrease @cite gomez2022hemothorax	</span>|<span class="success">	Decrease @cite gomez2022hemothorax	</span>|
|	Hemothorax: flow rate = 1.0 mL/s	|	Represents a massive traumatic hemothorax on the left lung side. Should lead to full collapse.	|	1500	|	2580	|<span class="success">	~1500  reduction from initial @cite zeiler2020hemothorax	</span>|<span class="success">	Decrease (atelectasis) @cite dogrul2020blunt	</span>|<span class="success">	Increase (tachypnea)  @cite gomez2022hemothorax	</span>|<span class="success">	Increase (tachycardia) @cite dogrul2020blunt	</span>|<span class="success">	Decrease (hypotension) @cite dogrul2020blunt	</span>|<span class="success">	Decrease (narrow) @cite gomez2022hemothorax	</span>|<span class="success">	Decrease (hypoxia)  @cite gomez2022hemothorax	</span>|
|	Hemothorax: flow rate = 0.0 mL/s; Apply Tube Thoracostomy	|	Stop the hemothorax and apply a tube thoracostomy on the left side of the chest to suck blood out of the pleural space.	|	2580	|	2880	|<span class="success">	No Change or slight increase	</span>|<span class="success">	Returning toward normal @cite dogrul2020blunt	</span>|<span class="success">	Returning toward normal @cite gomez2022hemothorax	</span>|<span class="success">	Returning toward normal @cite dogrul2020blunt	</span>|<span class="success">	Returning toward normal @cite dogrul2020blunt	</span>|<span class="success">	Returning toward normal @cite gomez2022hemothorax	</span>|<span class="success">	Returning toward normal  @cite gomez2022hemothorax	</span>|
|	Apply IV: compound = blood; bag volume = 1L; flow = 100 mL/min	|	Provide blood through an IV.	|	2880	|	4380	|<span class="success">	Increase	</span>|<span class="success">	Returning toward normal @cite dogrul2020blunt	</span>|<span class="success">	Returning toward normal @cite gomez2022hemothorax	</span>|<span class="success">	Returning toward normal @cite dogrul2020blunt	</span>|<span class="success">	Returning toward normal @cite dogrul2020blunt	</span>|<span class="success">	Returning toward normal @cite gomez2022hemothorax	</span>|<span class="success">	Returning toward normal  @cite gomez2022hemothorax	</span>|

### Mainstem Intubation

The right and left mainstem intubation actions were validated with a scenario that simulates mechanical ventilation with improper tracheal tube placement. In this scenario, the patient is injected with succinycholine, then mechanically ventilated through the placement of the endotracheal tube. During the intubation action, the tube is incorrectly placed, leading to the right and left mainstem intubation. The mainstem intubations are then set to the trachea for proper function. Common clinical vital signs were compared to validation data, as shown in @tableref {right_ms_int}.

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/MainstemIntubation_LeftLungVolume.jpg"><img src="./plots/Respiratory/MainstemIntubation_LeftLungVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/MainstemIntubation_TidalVolume.jpg"><img src="./plots/Respiratory/MainstemIntubation_TidalVolume.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/MainstemIntubation_RightLungVolume.jpg"><img src="./plots/Respiratory/MainstemIntubation_RightLungVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/MainstemIntubation_O2Sat.jpg"><img src="./plots/Respiratory/MainstemIntubation_O2Sat.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><a href="./plots/Respiratory/MainstemIntubationLegend.jpg"><img src="./plots/Respiratory/MainstemIntubationLegend.jpg" width="800"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {MainstemIntubation_Right}. Select outputs from the right mainstem intubation scenario.</i>
</center><br>

<center>
<i>@tabledef {right_ms_int}. Validation matrix for physiological responses due to varying severities of right mainstem intubation and correction.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time  (s)	|	Sampled Scenario Time  (s)	|	Respiration Rate (breaths/min)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Diastolic Pressure (mmHg)	|	Oxygen Saturation	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Turn on Anesthesia Machine and insert   Endotracheal (ET) tube in the right mainstem	|	Anesthesia machine is turned on to supply oxygen to the subject. ET tube is inserted, but it is set in the right mainstem.	|	60	|	180	|<span class="success">	16  (set by machine) 	</span>|<span class="success">	No Change @cite dukeSME; Increase above normal due to cardiovascular stress  @cite bergeronSME	</span>|<span class="success">	No Change @cite dukeSME; Increase above normal due to cardiovascular stress  @cite bergeronSME	</span>|<span class="success">	No Change @cite dukeSME; Increase above normal due to cardiovascular stress  @cite bergeronSME	</span>|<span class="success">	Continues to drop; 93-95% @cite bergeronSME	</span>|
|	Reset ET tube in the trachea.	|	 ET tube is adjusted to be set in the trachea (good depth).	|	180	|	480	|<span class="success">	16  (set by machine) 	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|
|	Turn on Anesthesia Machine and insert   Endotracheal (ET) tube in the left mainstem	|	ET tube is adjusted, but it is set in the left mainstem.	|	480	|	600	|<span class="success">	16  (set by machine) 	</span>|<span class="success">	No Change @cite dukeSME; Increase above normal due to cardiovascular stress  @cite bergeronSME	</span>|<span class="success">	No Change @cite dukeSME; Increase above normal due to cardiovascular stress  @cite bergeronSME	</span>|<span class="success">	No Change @cite dukeSME; Increase above normal due to cardiovascular stress  @cite bergeronSME	</span>|<span class="success">	Continues to drop; 93-95% @cite bergeronSME	</span>|
|	Reset ET tube in the trachea.	|	 ET tube is adjusted to be set in the trachea (good depth).	|	600	|	900	|<span class="success">	16  (set by machine) 	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|

### Esophageal Intubation

The esophageal intubation action was validated with a scenario with multiple intubation attempts, a failed esophageal intubation following by a successful endotracheal intubation. In this scenario, the patient is injected with succinycholine, followed by mechanical ventilation via an endotracheal tube. The first attempt at intubation results in an esophageal intubation. A second attempt is successful. 

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/EsophagealIntubation_TotalLungVolume.jpg"><img src="./plots/Respiratory/EsophagealIntubation_TotalLungVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/EsophagealIntubation_O2Sat.jpg"><img src="./plots/Respiratory/EsophagealIntubation_O2Sat.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/EsophagealIntubation_StomachInflow.jpg"><img src="./plots/Respiratory/EsophagealIntubation_StomachInflow.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/EsophagealIntubation_MAP.jpg"><img src="./plots/Respiratory/EsophagealIntubation_MAP.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><a href="./plots/Respiratory/EsophagealIntubationLegend.jpg"><img src="./plots/Respiratory/EsophagealIntubationLegend.jpg" width="800"></a></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {EsophagealIntubation}. Select outputs from the esophageal intubation scenario.</i>
</center><br>

<center>
<i>@tabledef {esoph_int}. Validation matrix for physiological responses due esophageal intubation and correction.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time  (s)	|	Sample Scenario Time  (s)	|	Respiration Rate  (breaths/min)	|	Heart Rate  (beats/min)	|	Systolic Pressure  (mmHg)	|	Diastolic Pressure  (mmHg)	|	Oxygen Saturation  (mmHg)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Esophageal Intubation	|	This represents failed  endotracheal intubation that led to esophageal intubation	|	60	|	120	|<span class="success">	Zero 	</span>|<span class="success">	 If undetected and O2 level drops significantly, Mild Increase @cite dukeSME	</span>|<span class="success">	 If undetected and O2 level drops significantly, Mild Increase @cite dukeSME	</span>|<span class="success">	 If undetected and O2 level drops significantly, Mild Increase @cite dukeSME	</span>|<span class="success">	continue to decrease along O2 curve 	</span>|
|	Successful Intubation	|	Represents successful endotracheal intubation	|	180	|	500	|<span class="success">	16  (set by machine) 	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|<span class="success">	Towards normal @cite bergeronSME	</span>|

### Asthma

The acute asthma action was validated against three scenarios: moderate (severity = 0.3), severe (severity = 0.7), and life-threatening (severity = 0.9). 

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/AsthmaAttackModerateAcute_TidalVolume.jpg"><img src="./plots/Respiratory/AsthmaAttackModerateAcute_TidalVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/AsthmaAttackModerateAcute_IERatio.jpg"><img src="./plots/Respiratory/AsthmaAttackModerateAcute_IERatio.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/AsthmaAttackModerateAcute_O2PP.jpg"><img src="./plots/Respiratory/AsthmaAttackModerateAcute_O2PP.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/AsthmaAttackModerateAcute_RR.jpg"><img src="./plots/Respiratory/AsthmaAttackModerateAcute_RR.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><center><a href="./plots/Respiratory/AsthmaAttackModerateAcuteLegend.jpg"><img src="./plots/Respiratory/AsthmaAttackModerateAcuteLegend.jpg" width="300"></a></center></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {AsthmaAttackModerate}. Select outputs from the moderate airway obstruction scenario.</i>
</center><br>

<center>
<i>@tabledef {mod_asthma}. Validation matrix for physiological responses due to moderate acute asthma and correction.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Trachea Flow - Peak Expiratory Flow  (L/min)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Moderate acute asthma attack (Severity = 0.3)	|		|	30	|	580	|<span class="success">	75% to 90% of normal @cite britishGuideline2014asthma	</span>|<span class="success">	Slight Increase @cite britishGuideline2014asthma	</span>|<span class="success">	 ~95% of normal @cite limmer2011emergency	</span>|<span class="success">	No Change @cite britishGuideline2014asthma	</span>|<span class="success">	No Change @cite papiris2001clinical	</span>|<span class="success">	No Change @cite britishGuideline2014asthma	</span>|<span class="success">	No Change @cite britishGuideline2014asthma	</span>|<span class="success">	No Change @cite britishGuideline2014asthma	</span>|<span class="success">	Decrease @cite van1991physical	</span>|
|	No asthma (Severity = 0.0)	|		|	580	|	775	|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/AsthmaAttackSevereAcute_TidalVolume.jpg"><img src="./plots/Respiratory/AsthmaAttackSevereAcute_TidalVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/AsthmaAttackSevereAcute_IERatio.jpg"><img src="./plots/Respiratory/AsthmaAttackSevereAcute_IERatio.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/AsthmaAttackSevereAcute_O2PP.jpg"><img src="./plots/Respiratory/AsthmaAttackSevereAcute_O2PP.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/AsthmaAttackSevereAcute_RR.jpg"><img src="./plots/Respiratory/AsthmaAttackSevereAcute_RR.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><center><a href="./plots/Respiratory/AsthmaAttackSevereAcuteLegend.jpg"><img src="./plots/Respiratory/AsthmaAttackSevereAcuteLegend.jpg" width="300"></a></center></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {AsthmaAttackSevere}. Select outputs from the severe asthma attack scenario.</i>
</center><br>

<center><br>
<i>@tabledef {sev_asthma}. Validation matrix for physiological responses due to severe acute asthma and correction.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Trachea Flow - Peak Expiratory Flow  (L/min)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Severe acute asthma attack (Severity = 0.7)	|		|	30	|	580	|<span class="success">	< 50% of Normal @cite papiris2001clinical, @cite britishGuideline2014asthma	</span>|<span class="warning">	> 25 @cite britishGuideline2014asthma	</span>|<span class="success">	< 60% of normal @cite limmer2011emergency	</span>|<span class="warning">	> 120 @cite papiris2001clinical; > 110 @cite britishGuideline2014asthma	</span>|<span class="danger">	Decrease Pulsus Paradoxus 85 mm Hg @cite papiris2001clinical	</span>|<span class="warning">	< 90% @cite papiris2001clinical; < 92% @cite britishGuideline2014asthma	</span>|<span class="warning">	Decrease Mild Hypoxemia @cite bergeronSME; < 60 mmHg @cite papiris2001clinical, @cite britishGuideline2014asthma	</span>|<span class="success">	Normal or Slight Increase > 45 mm Hg @cite papiris2001clinical, @cite britishGuideline2014asthma 	</span>|<span class="success">	Decrease @cite van1991physical	</span>|
|	No asthma (Severity = 0.0)	|		|	580	|	775	|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|

@htmlonly
<center>
<table border="0">
<tr>
    <td><a href="./plots/Respiratory/AsthmaAttackLifeThreateningAcute_TidalVolume.jpg"><img src="./plots/Respiratory/AsthmaAttackLifeThreateningAcute_TidalVolume.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/AsthmaAttackLifeThreateningAcute_IERatio.jpg"><img src="./plots/Respiratory/AsthmaAttackLifeThreateningAcute_IERatio.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Respiratory/AsthmaAttackLifeThreateningAcute_O2PP.jpg"><img src="./plots/Respiratory/AsthmaAttackLifeThreateningAcute_O2PP.jpg" width="550"></a></td>
    <td><a href="./plots/Respiratory/AsthmaAttackLifeThreateningAcute_RR.jpg"><img src="./plots/Respiratory/AsthmaAttackLifeThreateningAcute_RR.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><center><a href="./plots/Respiratory/AsthmaAttackLifeThreateningAcuteLegend.jpg"><img src="./plots/Respiratory/AsthmaAttackLifeThreateningAcuteLegend.jpg" width="300"></a></center></td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>@figuredef {AsthmaAttackLifeThreatening}. Select outputs from the life threatening asthma attack scenario.</i>
</center><br>

<center>
<i>@tabledef {max_asthma}. Validation matrix for physiological responses due to life threatening acute asthma and correction.</i>
</center>

|	Segment	|	Notes	|	Action Occurance Time (s)	|	Sampled Scenario Time (s)	|	Trachea Flow - Peak Expiratory Flow  (L/min)	|	Respiration Rate (breaths/min)	|	Tidal Volume (mL)	|	Heart Rate (beats/min)	|	Systolic Pressure (mmHg)	|	Oxygen Saturation	|	PaO2 (mmHg)	|	PaCO2 (mmHg)	|	IERatio	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Life threatening acute asthma attack (Severity = 0.9) 	|		|	30	|	580	|<span class="success">	< 33% of Normal @cite britishGuideline2014asthma	</span>|<span class="success">	> 25 @cite britishGuideline2014asthma	</span>|<span class="success">	< 60% of normal @cite limmer2011emergency	</span>|<span class="success">	> 120 @cite papiris2001clinical; > 110 @cite britishGuideline2014asthma Arrhythmia, @cite britishGuideline2014asthma	</span>|<span class="danger">	Decrease NO Pulsus Paradoxus 85 mm Hg @cite papiris2001clinical	</span>|<span class="success">	< 92% @cite britishGuideline2014asthma	</span>|<span class="success">	< 60 mmHg @cite britishGuideline2014asthma	</span>|<span class="warning">	Normal or Slight Increase > 45 mm Hg @cite papiris2001clinical, @cite britishGuideline2014asthma 	</span>|<span class="success">	Decrease @cite van1991physical	</span>|
|	No asthma (Severity = 0.0)	|		|	580	|	775	|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|<span class="success">	Normal	</span>|

### Dyspnea

Dyspnea or shortness of breath directly effects the achieved respiratory driver (breathing muscles) pressure amplitude and frequency.  The more severe the dyspnea, the lower the tidal volume / respiration rate. A maximum severity dyspnea is causes apnea.

<center><br>
<i>@tabledef {dyspnea}. Validation matrix for physiological responses due to varying severities of dyspnea.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Tidal Volume (mL)	|	Respiration Rate (breaths/min)	|
|	---	|	---	|	---	|	---	|	---	|	---	</span>|
|	Dysnea: Tidal Volulme Severity = 0.3	|	Mild amplitude effect	|	30	|	210	|<span class="success">	Decrease to ~70% of healthy	</span>|<span class="success">	Increase	</span>|
|	Dysnea: Tidal Volulme Severity = 0.6	|	Moderate amplitude effect	|	30	|	210	|<span class="success">	Decrease to ~30% of healthy	</span>|<span class="success">	Increase	</span>|
|	Dysnea: Tidal Volulme Severity = 1.0	|	Full amplitude effect	|	30	|	210	|<span class="success">	0	</span>|<span class="success">	0	</span>|
|	Dysnea: Severity = 0.0	|	Healthy	|	210	|	510	|<span class="success">	Return to Normal	</span>|<span class="success">	Return to Normal	</span>|
|	Dysnea: Respiration Rate Severity = 0.3	|	Mild frequency effect	|	30	|	210	|<span class="success">	Increase	</span>|<span class="success">	Decrease to ~70% of healthy	</span>|
|	Dysnea: Respiration Rate Severity = 0.6	|	Moderate frequency effect	|	30	|	210	|<span class="success">	Increase	</span>|<span class="success">	Decrease to ~30% of healthy	</span>|
|	Dysnea: Respiration Rate Severity = 1.0	|	Full frequency effect	|	30	|	210	|<span class="success">	0	</span>|<span class="success">	0	</span>|
|	Dysnea: Severity = 0.0	|	Healthy	|	210	|	510	|<span class="success">	Return to Normal	</span>|<span class="success">	Return to Normal	</span>|
|	Dysnea: Tidal Volume Severity = 0.3, Respiration Rate Severity = 0.3	|	Mild amplitude and frequency effect	|	30	|	210	|<span class="success">	Decrease to ~70% of healthy	</span>|<span class="success">	Decrease to ~70% of healthy	</span>|
|	Dysnea: Tidal Volume Severity = 0.6, Respiration Rate Severity = 0.6	|	Moderate amplitude and frequency effect	|	30	|	210	|<span class="success">	Decrease to ~30% of healthy	</span>|<span class="success">	Decrease to ~30% of healthy	</span>|
|	Dysnea: Tidal Volume Severity = 1.0, Respiration Rate Severity = 1.0	|	Full amplitude and frequency effect	|	30	|	210	|<span class="success">	0	</span>|<span class="success">	0	</span>|

### Supplemental Oxygen

The nasal cannula, simple mask, and non-rebreather mask are validated by applying the standard/default oxygen flow for each to the healthy standard male patient and looking at the oxygen fraction in the carina. All three supplemental oxygen devices match the expected fraction of inspired oxygen (FiO2).

<center><br>
<i>@tabledef {cannula}. Validation matrix for application of a nasal cannula.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Fraction of Inspired Oxygen (FiO2)	|
|	---	|	---	|	---	|	---	|	---	|
|	Supplemental Oxygen: Nasal Cannula	|	Default flow = 3.5 L/min	|	30	|	330	|<span class="success">	Increase to ~0.4 @cite korupolu2009early, @cite bailey2007early	</span>|

<center><br>
<i>@tabledef {simp_mask}. Validation matrix for application of a simple mask.</i>
</center>

|	Segment	|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Fraction of Inspired Oxygen (FiO2)	|
|	---	|	---	|	---	|	---	|	---	|
|	SupplementalOxygen: Simple Mask	|	Default flow = 7.5 L/min	|	30	|	330	|<span class="success">	Increase to ~0.5 @cite korupolu2009early, @cite bailey2007early	</span>|

<center><br>
<i>@tabledef {nrb_mask}. Validation matrix for application of a non-rebreather mask.</i>
</center>

|	Segment	|	Notes	|	Action Occurance Time (s)	|	Sampled Scenario Time (s)	|	Fraction of Inspired Oxygen (FiO2)	|
|	---	|	---	|	---	|	---	|	---	|
|	SupplementalOxygen: Non-Rebreather Mask	|	Default flow = 10.0 L/min	|	30	|	330	|<span class="success">	Increase to ~0.9 @cite korupolu2009early, @cite bailey2007early	</span>|

### Conscious Respiration 

Conscious respiration is validated as part of inhaler usage described in the %Inhaler methodology section @ref InhalerMethodology.

@anchor respiratory-assessmentvalidation
Validation - Assessments
------------------------

@anchor respiratory-conclusion
Conclusion
----------

Mathematical modeling of the pulmonary physiology dates back to the work of Gray (1945) and others, including Guyton and collaborators @cite milhorn1965mathematical . Since then several mathematical models have been developed that describe aspects of the pulmonary physiology in considerable detail. However, the scope and complexity of the human physiology demands a model that integrates the dynamic respiratory physiology with other systems such as cardiovascular, renal, and others. As a result, there is a need for an integrative modeling environment that serves as a tool for predicting aspects of the human physiology and pathology.

The %Respiratory Model provides a system-level computational model that allows for real-time simulation of normal pulmonary physiology and respiratory distresses. The model employs a circuit-based lumped parameter mathematical model that allows for the characterization of patient-specific respiratory mechanics at a relatively low computational burden. The coupling of the %Respiratory System with the Circulatory System through gas exchange and respiratory distress modifiers permits robust prediction of respiratory effects on hemodynamic variables. Accordingly, the current version of the %Respiratory System is capable of predicting respiratory and hemodynamic responses to various insults like airway obstruction, bronchoconstriction, pneumothorax, and others.

As part of a modeling tool for various clinical settings, the %Respiratory Model can be hooked up to the anesthesia machine for mechanical ventilation. During positive pressure ventilation, the %Respiratory Model permits the transport of inhaled volatile gases and others from the anesthesia machine to the circulatory system. Such an integrative approach allows for the examination of respiratory and cardiovascular responses to various stimuli and stresses as observed in practical application.

The %Respiratory System, along with the other systems in the engine, can serve as a training tool for exploring the dynamic interaction between physiological systems. Researchers can benefit from the integration of higher-fidelity modules into the existing model for more detailed and project-specific analysis. In its current form, the %Respiratory Model is adapted to a specific patient using physiological parameters selected from patient data. This limits the practical application of the model to the tested standard model. However, the common data model platform gives the user the ease and flexibility to select parameters to experiment their own patient setting.

@anchor respiratory-future
Future Work
===========

@anchor respiratory-coming
Coming Soon
-----------

We are actively designing and implementing work of breathing and respiratory muscle fatigue models.


@anchor respiratory-improvements
Recommended Improvements
------------------------

There are several more pathophysiology models that could be added, such as pulmonary edema and sepsis.

Some models would benefit by applying a distinct inhalation and exhalation resistance value. The current implementation uses single resistance values for both inhalation and exhalation.

It may be beneficial to further define pneumothorax as either spontaneous (action) or traumatic (condition), and add a simple (as opposed to tension) option. A tube thoracostomy intervention model could also be added.

A respiratory control mechanism for rhythmic breathing could be added to the engine. Rhythmic breathing is achieved by the respiratory neurons in the medullary respiratory center that give rise to activity in the efferent nerves ennervating the respiratory muscles. The neural control network that relates the respiratory neuron output to the mechanical airflow is not incorporated in the current version of the %Respiratory Model. The current version of the %Respiratory Model has succeeded in controlling the breathing cycle through parameters that  adjust the spontaneous breathing based on a chemical control feedback mechanism.  While this development allows robust chemical stimuli responses,  incorporation of the neural circuitry model that controls the ventilation process can provide a more accurate implementation of the %Respiratory System that includes behavioral control of breathing.

@anchor respiratory-appendices
Appendices
==========

Acronyms
--------

ARDS - Acute %Respiratory Distress Syndrome

COPD - Chronic Obstructive Pulmonary Disease

RV - Residual volume

ERV - Expiratory reserve volume

V<sub>T</sub> - Tidal volume

IRV - Inspiratory reserve volume

V<sub>C</sub>  - Vital capacity

IC - Inspiratory capacity

FRC - Functional residual capacity

TLC - Total lung capacity

RR - Respiration rate

V<sup><b>.</b></sup><sub>E</sub> - Minute ventilation

V<sup><b>.</b></sup><sub>A</sub> - Alveolar ventilation

PaO2 - Arterial blood oxygen partial pressure

FiO2 - Fraction of inspired oxygen

IE Ratio - Inspiratory:Expiratory Ratio

Data Model Implementation
-------------------------

@ref RespiratorySystemTable "Respiratory"
