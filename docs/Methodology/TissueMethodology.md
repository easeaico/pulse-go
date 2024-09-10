%Tissue Methodology {#TissueMethodology}
==========================

Overview
========

Abstract
--------
This %Tissue system manages the extravascular space. It handles substance transport between the organs and the blood vessels, and it computes substance storage, transformation (e.g. chemical conversion), clearance, and excretion. 

Introduction
------------
The %Tissue system is a low-resolution, mid-fidelity model of the tissues of the body. The system is mechanically tied to the [Cardiovascular](@ref CardiovascularMethodology) and [Respiratory](@ref RespiratoryMethodology) systems, and it interacts with the [Energy](@ref EnergyMethodology) and [Drugs](@ref DrugsMethodology) systems. The tissue system handles the non-advective transport of substances between the intravascular and extravascular spaces, as well as the conversion of substance (including chemical conversion of species and clearance/excretion). The metabolic production and consumption of substances takes place in the tissue system, and the tissues generate substances that are produced in the organs by any process or mode.

System Design
=============

Background and Scope
--------------------
Groups of cells in the body which share a common embryonic origin can be described collectively as a tissue. In the classical organizational hierarchy of organisms, tissues are at the level directly below organs, meaning that groups of tissues interacting to perform a function are an organ. There are four types of tissue in the human body: epithelial, connective, muscle, and nervous tissue. In the engine, the term tissue refers to the extravascular space of an organ. In other words, 'tissue', is a collective term which generally refers to the parenchyma.

@anchor tissue-data-flow
Data Flow
---------
Like the other systems, the %Tissue system uses the execution structure described in @ref SystemMethodology.

### Preprocess
#### Produce Albumin
The liver produces albumin as part of normal function. The engine does not have a dedicated hepatic model; therefore, a flat rate of albumin production is estimated in Tissue. The rate has been specified as 0.15 mg/s, as found in @cite Jarnum1972plasma. At each time step, the mass is added to the liver extracellular compartment. Hepatic albumin production is non-functional in the current implementation as the substance does not transport out of the tissue compartment. Improvements to the albumin production and transport model are [planned](@ref tissue-future).

### Process

#### Calculate Metabolic Consumption and Production
Substance production and consumption is computed based on the metabolic rate and the respiratory quotient.

#### Calculate Pulmonary Capillary Substance Transport
Gases are transferred from the lungs (alveoli) to the pulmonary capillaries and vice versa during this calculation. This allows for the transport of oxygen into the cardiovascular system from the ambient air, providing the required substances for metabolism. By the same process, carbon dioxide waste is removed from the %Cardiovascular System and moves through the %Respiratory System into the ambient air. 

#### Calculate Diffusion
%Substances move from the vascular space into and out of the extravascular or tissue space for metabolism, waste removal, and/ or clearance. This functionality moves gases across the membrane between the vascular and extravascular spaces using one or more of several diffusion models, discussed below.
- Perfusion Limited Diffusion
- Instant Diffusion
- Simple Diffusion
- Facilitated Diffusion
- Active Transport

#### Calculate Vital Signs
In this method the tissue volumes are summed in order to compute total body water. Body system level data is also set in this method. In the future, this method will compute and set tissue substance concentrations and trigger concentration-based events such as fasciculation.

### Post Process
At this time the there are no postprecess steps in the tissue system. 

### Assessments
Assessments are data collected and packaged to resemble a report or analysis that might be ordered by a physician. No assessments are associated with the %Tissue system.

Features, Capabilities, and Dependencies
----------------------------------------
The %Tissue system is a low-resolution, mid-fidelity model of the tissues of the body. One of the primary functions of the %Tissue system is to control the transport of substances between the tissues and the blood. There are several transport models which help the %Tissue system perform that function. Figure 2 provides an overview of the extravascular space and the various modes of substance transport between the blood and the tissues. The %Tissue system also handles the conversion of substance (i.e. metabolic consumption and production).

@anchor tissue-fig2
<a href="./Images/Tissue/tissueTransport.png"><img src="./Images/Tissue/tissueTransport.png" width="500"></a>
<center>
<i>Figure 2. The tissue compartment is partitioned into two distinct fluid spaces, and the non-fluid volume and mass are accounted for. There are several modes of transport between the spaces.</i>
</center><br>

### Bulk Flow and Advection
The movement of fluid between the intravascular and extravascular space is modeled using the @ref CircuitMethodology. Figure 3 shows a representative circuit diagram. In most cases, each tissue circuit node is connected to one and only one cardiovascular circuit node. However, the gut tissue compartment is a lumped representation of the abdominal viscera organ tissues, and thus the large intestine, small intestine, and splanchnic vascular circuit nodes all connect to the gut tissue circuit node.

<a href="./Images/Tissue/TissueCircuitExample.png"><img src="./Images/Tissue/TissueCircuitExample.png" width="500"></a>
<center>
<i>Figure 3. The tissue compartment is partitioned into two distinct fluid spaces, and the non-fluid volume and mass are accounted for. There are several modes of transport between the spaces.</i>
</center><br>

The volume in the tissue compartment is partitioned into the extracellular and intracellular space, as shown in [Figure 2](@ref tissue-fig2). The extracellular partition is connected to the circuit model, whereas the intercellular partition is seperate to allow for a non-linear control of the fluid flow dynamics. Although fluid moves according to the circuit model, there is no advective transport into the parenchyma. All substance transport into the tissue fluid space is simulated using one or more of the transport modes described below.

@anchor tissue-variability
### Patient Variability
The %Tissue system is heavily dependent on the patient configuration. Fluid volume distributions and parenchyma masses both depend heavily on the patient sex, height, weight, and body fat fraction. Transport properties are also affected by patient variability. For example, permeability coefficients are computed from membrane permeability and membrane surface area, where the surface area is a function of the tissue mass, which in turn is a function of the patient weight. A detailed discussion of patient configuration and variability is available in the @ref PatientMethodology report.

@anchor tissue-perfusionLimitedDiffusion
## Perfusion-Limited Diffusion
Perfusion-limited diffusion is a technique for describing drug kinetics in physiology-based pharmacokintic models. Partition coefficents are used to compute the amount of a drug crossing a membrane at a given perfusion rate. The partition coefficients are calculated based on the physical chemical properties of the drug, the tissue properties of the organ, and the blood properties. They represent a specific substance&rsquo;s affinity for moving across the blood-tissue partition. The engine uses this methodology to simulate drug diffusion, and details of the partition coefficient calculation can be found in the @ref DrugsMethodology. All current drugs in the engine use perfusion-limited diffusion as found in @cite khalil2011physiologically @cite huisinga2012modeling. In the [future](@ref drugs-future), permeability-limited diffusion could be used. Equation 1 shows the calculation used to move mass from the vascular to the tissue and vice versa for perfusion-limited diffusion @cite khalil2011physiologically .

\f[\Delta M = Q_{T} * C_{V} - \frac{Q_{T} * C_{T}}{K_{P}} \f]
<center>
*Equation 1.*
</center><br>

Where <i>&Delta;M</i> is the change in mass due to diffusion, *Q<sub>T</sub>* is the blood flow to the organ, *C<sub>V</sub>* is the concentration of the drug in the organ vasculature, *C<sub>T</sub>* is the concentration of the drug in the organ tissue, and *K<sub>P</sub>* is the partition coefficient for the drug and organ. This calculation is performed for each drug or substance and each tissue organ/compartment.

@anchor tissue-diffusion
## Gradient Dependent Transport Processes

### Gas Exchange - Alveoli Transfer
At the alveoli-pulmonary capillary interface, oxygen diffuses from the alveoli into the pulmonary capillaries, while carbon dioxide diffuses from the pulmonary capillaries into the alveoli. In reality, gas exchange at the alveoli is a multi-step process in space, where gases dissolve into liquid according to Henry's law and diffuse through liquid and across membranes according to Fick's law. In the model, alveolar gas exchange is driven by the partial pressure differential between the pulmonary capillaries and the alveoli in a one-step process, as shown in Figure 4. The partial pressures of each gas in the capillaries are calculated using Equation 2, while the partial pressures of each gas in the alveoli are calculated using Equation 3.

<a href="./Images/Tissue/AlveolarDiffusion.png"><img src="./Images/Tissue/AlveolarDiffusion.png" width="700"></a>
<center>
<i>Figure 4. Alveolar gas exchange is a single-step, lumped-diffusion process driven by a partial pressure gradient, where the partial pressures are computed using Equations 2 and 3.</i>
</center><br>

\f[P_{P} = \frac{C}{d * C_{S}} \f]
<center>
*Equation 2.*
</center><br>

\f[P_{P} = P * V_{f} \f]
<center>
*Equation 3.*
</center><br>

Where, *P<sub>p</sub>* is the partial pressure, *C* is the concentration, *d* is the density, *C<sub>s</sub>* is the solubility coefficient, *P* is the total pressure, and *V<sub>f</sub>* is the volume fraction.

The diffusion rate is calculated using Equation 4 @cite guyton2006medical . 

\f[\dot{D} = \frac{D_{co} * C_{D} * \Delta P_{P} * SA_{a}}{D_{d}} \f]
<center>
*Equation 4.*
</center><br>

Where *D<sub>co</sub>* is the diffusing capacity of oxygen, *C<sub>D</sub>* is the relative diffusion coefficient, \delta *P<sub>p</sub>* is the partial pressure differential between the alveoli and the capillaries, *SA<sub>a</sub>* is the surface area of the alveoli, and *D<sub>d</sub>* is the diffusion distance. The surface area of the alveoli for an individual patient is related to the standard alveoli surface area and the patient&rsquo;s total lung capacity. This calculation is shown in Equation 5.

\f[SA_{a} = \frac{TLC_{p}}{TLC_{s}} * SA_{as} \f]
<center>
*Equation 5.*
</center><br>

Where *TLC<sub>p</sub>* is the total lung capacity of the patient, as specified in the patient file (@ref PatientData). *TLC<sub>s</sub>* is the standard healthy total lung capacity of 5.8 L @cite guyton2006medical . The *SA<sub>as</sub>* is standard alveoli surface area of 70 square meters @cite guyton2006medical. For more information about patient variability, please see the @ref PatientMethodology report.

The mass diffused at each time step is calculated using Equation 6. This mass is either added or removed from the pulmonary capillaries and the corresponding volume is either added or removed from the alveoli.

\f[D_{m} = \dot{D} * \Delta t * d \f]
<center>
*Equation 6.*
</center><br>

### Instant Diffusion
Some substances are able to diffuse across biological membranes at a rate that ensures concentration equilibrium within one engine time step. The instant diffusion model is included in the %Tissue system in order to simulate transport processes that fully evolve in a time period much smaller than the engine time step. All of the gases are transported by instant diffusion. 

### Simple Diffusion
Simple diffusion is an implementation of Fick's law in one dimension with a known constant distance. In this case, Fick's law can be described by Equation 7.

\f[J_{X} = P_{x} * \left([X]_{v} - [X]_{t} \right) \f]
<center>
*Equation 7.*
</center><br>
Where *J<sub>x</sub>* is the mass flux (mass per area-time) of substance *X*, *[X]<sub>v,t</sub>* is the concentration of substance X in compartment v (or t), and *P<sub>x</sub>* is a proportionality constant defining the permeability. The flux is multiplied by an area to obtain a rate of mass transfer. It is incredibly difficult to experimentally determine the capillary surface area for a given tissue, and it may be impossible to experimentally determine the total cellular membrane surface area. Additionally, lumped tissue models can be difficult to delineate. In the engine, the capillary and cellular membrane surface areas are assumed to be proportional to the mass of a given organ or tissue group, such that the mass transfered in one time step (*D<sub>m</sub>*) may be computed by Equation 8, where *k* is the empirically-determined constant relating the tissue mass (*m<sub>t</sub>*) to the surface area.

\f[ D_{m} = k * m_{t} * J_{X} * \Delta t \f]
<center>
*Equation 8.*
</center><br>

### Facilitated Diffusion
Facilitated diffusion uses Michaelis-Menten kinetics to model the facilitated transport across a membrane. Note that this type of diffusion does not require energy and it is still a gradient-based transport mode. In contrast to simple diffusion, where substance flux can continue to increase with the concentration gradient, the flux is asymptotic in facilitated diffusion. The flux limit reflex a saturation of the membrane transporter mechanisms. However, at smaller concentration gradients, substance flux is higher in facilitated diffusion than with simple Fick's law diffusion. Figure 5 demonstrates the difference in flux between facilitated and simple diffusion. The mass flux given by Michaelis-Menten kinetics is computed using Equation , where *J<sub>max</sub>* is the maximum flux and *K<sub>m</sub> is the Michaelis constant.

\f[ J_{X} = \frac{\left([X]_{v} - [X]_{t} \right) * J_{max}}{K_{m} * \left([X]_{v} - [X]_{t} \right)} \f]
<center>
*Equation 9.*
</center><br>

<a href="./plots/Tissue/FluxVsGradient.jpg"><img src="./plots/Tissue/FluxVsGradient.jpg" width="900"></a>
<center>
<i>Figure 5. In simple diffusion based on Fick's law, the substance flux is directly proportional to the concentration gradient. There is no upper limit to transport. In contrast, the flux is asymptotic in facilitated diffusion, reflecting a saturation of transporters in the membrane. These two transport processes can work in concert.</i>
</center><br>

## Gradient Independent Transport Processes
### Active Transport
Active transport is a coarse model of the energy-requiring transport processes (e.g. the sodium-potassium pump). The active transport functionality is currently inactive while work continues on comprehensive substance handling. When complete, active transport will use energy (through feedback to consumption rates of energy-containing substances) to move substances across barriers.

@anchor tissue-metabolic-production
## Metabolic Production and Consumption
Metabolism is simulated by production and consumption of substances. The basis for the metabolic production and consumption calculations is the  non-protein respiratory quotient (RQ). This value is used to determine the fraction of the metabolic energy that is produced through carbohydrate (glucose) consumption. The curve is determined from data obtained from Gropper and Smith @cite gropper2013nutrition and is displayed in Figure 6.

<a href="./plots/Tissue/Macronutrient_Consumption.jpg"><img src="./plots/Tissue/Macronutrient_Consumption.jpg" width="900"></a>
<center>
<i>Figure 6. The Non-Protein %Respiratory Quotient displays the relationship between the fraction of the metabolism that is generated through carbohydrate metabolism vs. the fraction that is generated via fat metabolism. There is an inverse relationship between the fractions since the metabolic rate is entirely derived from glucose at a RQ of 1.</i>
</center><br>

The fraction of carbohydrates consumed determines the removal of glucose, with the remaining portion of the fraction determining the removal of tristearin. Glucose is removed in one of two pathways: aerobic metabolism or anaerobic metabolism. The ratio of aerobic to anaerobic metabolism is determined from an anaerobic weighting factor. This factor is defined as the current oxygen partial pressure in the tissue over a resting value of 40 mmHg @cite carreau2011oxygen. The anaerobic weight is shown in Equation 0.

\f[w(t) = \frac{P_{O_{2}}(t)}{P_{O_{2,rest}}} \f]
<center>
*Equation 10.*
</center><br>
After the fractions of the metabolism have been defined, the exact consumption rates can be determined from molar ratios of nutrient input to ATP (energy) output. Table 1 shows the molar input to output ratios.
<br><center>
*Table 1. The molar ratio of nutrient to output of ATP @cite gropper2013nutrition. This gives ratios of the molar amount of each nutrient required to the moles of ATP produced*
</center>
|Molar Ratio                              		|Value                     |
|------------------------                     	|------------------------  |
|Glucose To ATP                           		|1/38                      |
|Glucose To Lactic Acid                  		|1/2                       |
|Oxygen To ATP from Glucose Consumption         |6/38                      |
|Carbon Dioxide To ATP from Glucose Consumption |6/38                      |
|Oxygen To ATP from Lipid Consumption           |163/768                   |
|Carbon Dioxide To ATP from Lipid Consumption   |114/768                   |
|Lactate To ATP                                 |1/36                      |
|Acetoacetate To ATP                            |1/24                      |

The substance to ATP ratios are used to determine the consumption and production rates in the tissues, given in units of moles per time in Table 2. 
<br><center>
*Table 2. The equations for nutrient and gas consumption/production are displayed for the following metabolic substances: glucose, lipids, acetoacetic acid,
lactic acid, oxygen, and carbon dioxide.*
</center>
| Substance | Consumption | Production | Tissues |
| :-------: | :---------: | :--------: | :-----: |
| Glucose | \f[ F_{g}*\left(w_{O_{2}}*\frac{ \dot{M}_{ATP}}{38} + \left(1-w_{O_{2}} \right) * \frac{ \dot{M}_{ATP}}{2} \right) \f] | N/A | All |
| Lipid | \f[ \left(1-F_{g} \right) \left( \frac{2}{768} \right) \dot{M}_{ATP} \f] | N/A | All |
| Acetoacetate | \f[ \left( \frac{1}{24} \right) \dot{M}_{ATP} \f] | \f[ \dot{M}_{ketone} = \dot{M}_{ketone,base} + \left(1-F_{g} \right) \dot{M}_{ketone,max} \f] | Production: Liver <br> Consumption: Myocardium, Brain, Muscle |
| Lactate | \f[ \left( \frac{1}{36} \right) \dot{M}_{ATP} \f] | \f[ F_{g} * \left(1-w_{O_{2}} \right) \dot{M}_{ATP} \f] | Production: All <br> Consumption: Myocardium, Brain |
| Oxygen | \f[ F_{g} w_{O_{2}} \frac{6}{38} \dot{M}_{ATP} + \left(1-F_{g} \right) \frac{163}{768} \dot{M}_{ATP} \f] | N/A | All |
| Carbon Dioxide | N/A | \f[ F_{g} w_{O_{2}} \frac{6}{38} \dot{M}_{ATP} + \left(1-F_{g} \right) \frac{114}{768} \dot{M}_{ATP} \f] | All |

Assumptions and Limitations
---------------------------
Proteins are large molecules which take up space. About 7% of plasma volume is due to proteins. Proteins also have a net negative charge. in reality, diffusion depends on the elctrochemical gradient, not just the chemical gradient. With the exception of the perfusion-limited diffusion model, the diffusion models do not account for the entire elecrochemical gradient or the volume of the protein in the plasma (i.e. no "plasma water").

The diffusional exchange of water between the capillaries and extravascular space amounts to as much as 80,000 liters per day. Convective capillary exchange is much less, on the order of 16 liters per day. The diffusional exchange of water is not modeled in the engine.

Conditions
----------

### Dehydration

In the dehydration condition model, a severity value is utilized to initialize patients with various homeostatic levels/severities of dehydration. This model simulates nearly isotonic dehydration, typically resulting from excessive sweating. The process is carried out by iterating through tissue vasculature, intracellular, and extracellular spaces, removing a consistent fraction of fluid from each. Additionally, the removal of substances mimics those lost through perspiration. While the mineral content in sweat varies, the model applies generally accepted concentrations:

Electrolytes:
 - Sodium: 1.0 g/L (0.46-1.84 g/L - most abundant, higher levels with more intense exercise)
 - Chloride: 1.5 g/L (0.71-2.84 g/L - balances sodium concentration)
 - Potassium: 0.2 g/L (0.16-0.39 g/L - concentrations decreasing during prolonged exercise)
 - Calcium: 0.02 g/L (0-0.12 g/L - dropping significantly with increased sweat rate)
Other solutes:
 - Lactate: 0.01 g/L (0-0.02 g/L - increases with exercise intensity and reflects muscle energy metabolism)
 - Urea: 0.003 g/L (0.002-0.01 g/L - waste product of protein breakdown, generally higher than in blood plasma)

For severity mapping, dehydration categories are defined based on the fraction of body weight lost @cite gellert2015signs:
 - Imminent: 0.22-1% of body weight
 - Mild: 1-2% of body weight
 - Moderate: 2-5% of body weight
 - Severe: 5-10% of body weight
 - Extreme: > 10% of body weight
 - Fatal: > 15% of body weight

These values were mapped to corresponding severities to derive a best-fit equation for total fluid loss, given a specific severity input. Figure 7 illustrates the applied equation in the model.

<a href="./Images/Tissue/DehydrationSeverityMapping.png"><img src="./Images/Tissue/DehydrationSeverityMapping.png" width="500"></a>
<center>
<i>Figure 7. The dehydration severity mapping.</i>
</center><br>

Special attention is given to fluid removal from the cardiovascular and tissue circuits, ensuring accurate handling of compliances due to their temporal nature. Pressure values from the previous timestep are adjusted to align with the volume change, maintaining the same pressure-volume relationship as if the fluid loss occurred over an extended period.

Metabolic consumption and production vary during dehydration due to reduced tissue perfusion from decreased blood volumes. In the model, this is represented by adjusting the local ATP use rate for each tissue compartment, based on the blood volume reduction relative to a healthy patient. This leads to increased CO2 production, ultimately causing vital sign changes through chemoreceptor feedback.

For more information on the application of the dehydration model, see the @ref EnergySystem and @ref NervousSystem documentation.


Actions
-------
At this time, there are no insults or interventions associated with the %Tissue system. Other system actions can affect the diffusion properties or other transports by modifying the diffusion surface area. An example of this is found in the @ref RespiratoryMethodology for pneumonia. As the alveoli fill with fluid, they are unable to participate in gas exchange. This reduces the alveoli surface area, which leads to a reduction of available oxygen in the @ref CardiovascularSystem and the @ref EnergySystem. 

Events
------

- Mild Dehydration: A loss of more fluid than is taken in (More than 3% loss of healthy/initial fluid mass)
- Moderate Dehydration: A loss of more fluid than is taken in (More than 5% loss of healthy/initial fluid mass)
- Severe Dehydration: A loss of more fluid than is taken in (More than 10% loss of healthy/initial fluid mass)
- Hyponatremia: Serum sodium concentration (i.e., blood plasma) < 135 mEq/L
- Hypernatremia: Serum sodium concentration (i.e, blood plasma) > 145 mEq/L
- Fasciculation: Event currently inactive while substance handling is improved

Results and Conclusions
=======================

Verification
-------------
%Verification of the diffusion methods is achieved through several units tests. One of the simple diffusion unit tests was used to generate data for Figure 8. The figure shows the time-evolution of the concentrations of four different compartments. Table 3 shows the initial conditions. Note that the units are arbitrary, thus not shown. The red, blue, and green compartment all share a boundary with the yellow compartment, but not with each other.
<br><center>
*Table 3. Initial conditions for a four compartment simple diffusion unit test.*
</center>
| Compartment | Volume | Mass | Concentration |
| ----------- | ------ | ---- | ------------- |
| Red         | 50.0   | 2.0  | 0.04          |
| Blue        | 10.0   | 2.5  | 0.20          |
| Green       | 20.0   | 10   | 0.50          |
| Yellow      | 50.0   | 0.0  | 0.00          |

<a href="./plots/Tissue/ConcentrationEquilibration.jpg"><img src="./plots/Tissue/ConcentrationEquilibration.jpg" width="900"></a>
<center>
<i>Figure 8. Four compartments start with different concentrations which equilibrate after some time. Initial conditions are shown in Table 3 above.</i>
</center><br>

Validation - Resting Physiologic State
--------------------------------------
The tissue system volumes are validated using data from @cite valentin2002icrp.  

<br><center>
*Table 4. Validation of the resting physiologic state comparison of system-level outputs from the engine to referenced values. System-level outputs show favorable agreement with validation data.*
</center>

<b>Standard Male</b>

@insert ./test_results/tables/Tissue-StandardMaleValidationTable.md

<b>Standard Female</b>

@insert ./test_results/tables/Tissue-StandardFemaleValidationTable.md

<br><center>
*Table 5. Validation of the resting physiologic state comparison of compartment-level outputs from the engine to referenced values. The compartments are currently validated on a flow/volume basis. Flows and most of the volumes show good agreement with validation values.*
</center>

<b>Standard Male</b>

@insert ./test_results/tables/TissueCompartments-StandardMaleValidationTable.md

<b>Standard Female</b>

@insert ./test_results/tables/TissueCompartments-StandardFemaleValidationTable.md

More validation of this system can be found in the system outputs of all other systems, e.g., the oxygen and carbon dioxide saturation, the blood pH, and the bicarbonate concentration values are found in the @ref BloodChemistryMethodology and the alveoli oxygen and carbon dioxide partial pressures are found in the @ref RespiratoryMethodology.

Validation - Actions and Conditions
-----------------------

The dehydration model was validated using three distinct scenarios representing mild, moderate, and severe dehydration. The simulation outputs for each scenario were rigorously validated. Quantitative validation was applied wherever possible, with qualitative validation employed in other areas by comparing the engine output against expected trends and values. Table 6 presents the results for a standard healthy patient alongside those for mild, moderate, and severe dehydration. The validation is specified with a color-coded comparison, with green indicating good agreement with trends/values, yellow indicating moderate agreement with trends/values, and red indicating poor agreement with trends/values. The dehydration model outputs shows good overall agreement with the predicted trends.

<br><center>
*Table 6. Results for the dehydration validation scenarios.*
</center>

| Parameter                         | Healthy Value | Mild Value | Mild Expected                                                      | Moderate Value | Moderate Expected                                                 | Severe Value | Severe Expected                                                   |
|-----------------------------------|---------------|------------|--------------------------------------------------------------------|----------------|------------------------------------------------------------------|--------------|-------------------------------------------------------------------|
| Total Fluid Volume (L)            | 39.6          | 36.8       | <span class="success">>3% loss (by weight) @cite gellert2015signs</span> | 34.3           | <span class="success">>5% loss (by weight) @cite gellert2015signs</span> | 31.0         | <span class="success">>10% loss (by weight) @cite gellert2015signs</span> |
| Blood Volume (L)                  | 5.5           | 5.12       | <span class="success">Decrease @cite murray1996dehydration</span>       | 4.76           | <span class="success">Decrease @cite murray1996dehydration</span>      | 4.29         | <span class="success">Decrease @cite murray1996dehydration</span>      |
| Heart Rate (1/min)                | 71.9          | 78.5       | <span class="success">< 100 bpm @cite gellert2015signs; Increase @cite murray1996dehydration</span> | 87.3           | <span class="success">< 100 bpm @cite gellert2015signs; Increase @cite murray1996dehydration</span> | 104          | <span class="success">Tachycardia @cite gellert2015signs; Fast @cite levine2015empirically; Increase @cite murray1996dehydration</span> |
| Respiration Rate (1/min)          | 12.1          | 12.8       | Not provided                                                       | 13.5           | <span class="success">Rapid breathing @cite dellamano2016dehydration</span> | 14.7         | <span class="success">Increase @cite gellert2015signs @cite dellamano2016dehydration</span> |
| Tidal Volume (mL)                 | 528           | 566        | Not provided                                                       | 603            | <span class="success">Deep breathing @cite dellamano2016dehydration</span> | 686          | <span class="success">Deep breathing @cite levine2015empirically</span> |
| Pulse Pressure (mmHg)             | 40.7          | 34.1       | <span class="danger">No change @cite gellert2015signs</span>           | 28.4           | <span class="danger">No change @cite gellert2015signs</span>          | 22.1         | <span class="success">Narrow @cite gellert2015signs</span>             |
| Systolic Arterial Pressure (mmHg) | 114           | 111        | Not provided                                                       | 108            | Not provided                                                      | 104          | <span class="success">Decreased or low @cite dellamano2016dehydration</span> |
| Cardiac Output (L/min)            | 5.78          | 5.16       | <span class="success">No change @cite gellert2015signs; Decrease @cite murray1996dehydration</span> | 4.66           | <span class="success">No change @cite gellert2015signs; Decrease @cite murray1996dehydration</span> | 4.23         | <span class="success">Decreased @cite gellert2015signs; Decrease @cite murray1996dehydration</span> |
| Central Venous Pressure (mmHg)    | 4.67          | 4.06       | <span class="success">Decrease @cite murray1996dehydration</span>       | 3.74           | <span class="success">Decrease @cite murray1996dehydration</span>      | 3.2          | <span class="success">Decrease @cite murray1996dehydration</span>      |
| Heart Stroke Volume (mL)          | 80.4          | 65.7       | <span class="success">Decrease @cite murray1996dehydration</span>       | 53.4           | <span class="success">Decrease @cite murray1996dehydration</span>      | 40.7         | <span class="success">Decrease @cite murray1996dehydration</span>      |
| Core Temperature (°C)             | 37.1          | 38.6       | <span class="success">Increase @cite murray1996dehydration</span>       | 39.2           | <span class="success">Increase @cite murray1996dehydration</span>      | 39.7         | <span class="success">Increase @cite murray1996dehydration</span>      |
| Sweat Rate (mg/min)               | 335           | 4,500      | <span class="danger">Decrease @cite murray1996dehydration</span>        | 3,430          | <span class="warning">Decrease @cite murray1996dehydration</span>      | 1,990        | <span class="warning">Decreased @cite gellert2015signs; Decrease @cite murray1996dehydration</span> |
| Plasma Osmolality (mOsm/kg)       | 246           | 258        | <span class="success">Increase @cite murray1996dehydration</span>       | 268            | <span class="success">Increase @cite murray1996dehydration</span>      | 283          | <span class="success">Increase @cite murray1996dehydration</span>      |
| Urine Color                       | Yellow        | Yellow     | <span class="success">Light yellow @cite dellamano2016dehydration</span> | Dark Yellow    | <span class="success">Dark yellow to brown @cite gellert2015signs @cite dellamano2016dehydration</span> | Dark Yellow  | <span class="success">Dark yellow to brown @cite gellert2015signs @cite dellamano2016dehydration</span> |


@anchor tissue-future
Future Work
===========
Coming Soon
-----------
- Improved blood glucose and blood triglyceride dynamic model
- Albumin transport model to affect blood albumin concentration based on hepatic production

Recommended Improvements
------------------------
- Permeability-limited diffusion model
- Endogenous carbon monoxide production

Appendices
==========

Data Model Implementation
-------------------------
@ref TissueSystemTable "Tissue"
