%Endocrine Methodology {#EndocrineMethodology}
=====================

Overview
========
@anchor endrocrine-abstract
Abstract
--------

The %Endocrine System is a basic system implementation of endocrine signaling. There are currently three hormones included in the %Endocrine system: norepinephrine, epinephrine, and insulin. The effects of norepinephrine and epinephrine are modeled by leveraging the [pharmacodynamics](@ref DrugsMethodology) model. The insulin model is under construction, and when it is finished, insulin will have a regulatory effect on the concentration of glucose in the blood. Functionality that is in the development stage now will enable future releases of the endocrine system to affect:
-   Metabolic function
-   %Renal function
-   Ion regulation

System Design
=============
@anchor endrocrine-background
Background and Scope
--------------------

The %Endocrine System&rsquo;s primary function is to release hormones to maintain homeostasis and regulate body functions, including metabolism, growth and development, reproduction, and behavior. The human body&rsquo;s multiple endocrine glands release a variety of hormones. Different endocrine glands respond to stimuli by synthesizing, releasing, or inhibiting the release of hormones. In general, hormone release is described by a negative feedback mechanism, meaning the effects of the hormone on the physiology cause a cessation of further hormone release. Positive feedback can occur with hormone release, which leads to more hormone release as the physiologic effects of the hormone are felt. Positive feedback mechanisms are generally isolated to sexual and reproductive hormones @cite guyton2006medical. The %Endocrine System will affect metabolic function, renal function, ion regulation, and sympathetic stimulation. The engine does not attempt to model growth, sexual, or reproductive hormones.

@anchor endrocrine-dataflow
Data Flow
---------

### Reset, Conditions, and Initialization
Hormone values are initialized where necessary during reset. There are no conditions in the %Endocrine System.

### Preprocess
There is no system-specific function for Preprocess in the %Endocrine System.

### Process
During Process, norpeinephrine and epinephrine are released. The release represents the basal production rate of the two hormones. The epinephrine production rate is scale to represent the sympathetic response associated with exercise and/or stress. Norepinephrine production is inversely proportional to the epinephrine production to represent the parasympathetic response. The rate of insulin synthesis is calculated and insulin is added to the system.

### Post Process
There is no system specific function for Post Process in the %Endocrine System.

### Assessments
There are no assessments in the %Endocrine System.

@anchor endrocrine-features
Features, Capabilities, and Dependencies
----------------------------------------

The %Endocrine System currently contains only two hormones: epinephrine and insulin. Both hormones are released endogenously and cleared at a specified rate. The release rate of insulin is modified by the concentration of glucose in the blood, and the release rate of epinephrine is perturbed from the basal rate by two actions: exercise and acute stress.

### Insulin
Insulin is secreted by the pancreatic beta cells in response to increased blood glucose.
The model utilized for insulin synthesis is a linear function capturing the insulin/glucose relationship described in Polonsky @cite polonsky1987insulin.
The synthesis rate is defined by:

\f[f(I)= (5.357 * G) - 328.56 \f] 
<center>
*Equation 1.*
</center><br>
Where <i>G</i> is glucose concentration of the blood. 
This linear function is valid for glucose levels of 80-150 mg/dL, and will compute an insulin synthesis rate between 100-475 pmol/min accordingly.

Because the %Cardiovascular System does not contain a pancreas compartment, the insulin is synthesized directly in the splanchnic.

Insulin does not have any effects attributable to pharmacodynamic model. Its only purpose is to modulate the consumption of glucose.

### Epinephrine
Epinephrine is released by the adrenal medulla at a basal endogenous rate of approximately 0.18 (&mu;g/min) @cite best1982release. To enable patient variability, the engine uses a mass-normalized basal epinephrine release rate of about 0.003 (&mu;g/kg-min). Because the engine does not have an adrenal gland compartment, epinephrine is released equally from the kidneys' efferent arterioles. The epinephrine clearance rate is constant and will not change with concentration (note that the actual amount cleared is a function of the concentration as well as the hemodynamics). A basal clearance rate of 68.7 (mL/kg-min) was computed from the basal release rate and by assuming a steady-state normal concentration of 0.032 (&mu;g/L) @cite wortsman1984adrenomedullary. The basal clearance rate is a property of the epinephrine substance definition; therefore, the clearance rate can be adjusted in the epinephrine substance file to achieve a desired steady-state concentration. A wide range of normal basal epinephrine concentrations are reported in literature @cite wortsman1984adrenomedullary @cite stratton1985hemodynamic @cite stein1998basal @cite penesova2008role @cite Zauner2000resting. 

Two stimuli, exercise and acute stress, can modify the epinephrine release rate. 

### Norepinephrine 
Norepinephrine is released  as part of the body's parasympathetic response. The norepinephrine clearance rate is estimated to be 22-27 (mL/kg-min) in @cite Johnston2004pharmacokinetics; however, this is in critically injured head trauma patients. We calculated a basal production rate based on this value, resulting in approximately 0.009 (&mu;g/kg-min) as a mass normalized production rate. Norepinephrine is currently released directly into the bloodstream. The basal concentration rate and those for different infusion rates were tested and validated using the data found in @cite Ensinger1992relationship. The clearance and production rates were adjusted slightly to meet this validation data. More information on the validation can be found in @ref DrugsMethodology and @ref BloodChemistryMethodology.

Two stimuli, exercise and acute stress, can modify the norepinephrine release rate. The norepinephrine release is considered to be inversely proportional to the epinephrine release rate during this circumstances. The epinephrine release was validated as noted below.

#### Exercise
The increase in epinephrine release as a function of above-basal exercise was developed using data in @cite stratton1985hemodynamic and @cite tidgren1991renal. We assume that the epinephrine clearance rate is constant; therefore, the fractional increase in epinephrine concentration described in @cite stratton1985hemodynamic and @cite tidgren1991renal can be assumed to be due to a similar fractional increase in release rate. Using that assumption, we fit a logistic function to the basal-normalized epinephrine steady-state concentrations during exercise presented in @cite tidgren1991renal. The release modifier varies from 1 to 19.75, as shown in Figure 1, meaning that the epinephrine release rate will be 19.75 times the basal release rate with maximal exercise. The model is implemented by first computing the above-basal metabolic rate and then using the generic logistic function with the appropriate parameter values to compute the release rate multiplier.

<a href="./plots/Endocrine/EpiExercise.jpg"><a href="./plots/Endocrine/EpiExercise.jpg"><img src="./plots/Endocrine/EpiExercise.jpg" width="600"></a></a>
<center>
*Figure 1. The increase in epinephrine release during exercise is computed as a fraction of the basal rate.*
</center><br>


The whole-body physiological response to exercise is compared to empirical data in the [exercise validation](@ref energy-exercise-validation) section of the @ref EnergyMethodology report.

#### Acute Stress
Acute Stress is an action that causes an increase in the basal production of epinephrine. For details see the [Actions](@ref endocrine-actions) section of this document.

The pharmacodynamic effects of epinephrine are applied using the sigmoid 'E<sub>max</sub>' model described in [pharmacodynamics](@ref drugs-pharmacodynamics) section of the @ref DrugsMethodology report. Maximum effects for the model were derived from data in @cite clutter1980epinephrine, @cite stratton1985hemodynamic, and @cite tidgren1991renal. Note that these researchers were examining the relationships between epinephrine and exercise, and not the relationships between epinephrine and acute stress. For that reason, the model of epinephrine release during the acute stress action is more phenomenological.

@anchor endocrine-dependencies
### Dependencies

The %Endocrine System is dependent on the key systems of the engine. The %Endocrine System is dependent on the metabolic rate determined in the %Energy System. The insulin synthesis rate is dependent on the concentration of glucose in the %Cardiovascular System.

@anchor endrocrine-assumptions
Assumptions and Limitations
---------------------------

Epinephrine has a variety of effects in the human body, but all of the effects do not occur at the same epinephrine concentration thresholds. For example, a patient with a slowly increasing blood concentration of epinephrine would experience an elevated heart rate before they experience a decrease in diastolic pressure @cite clutter1980epinephrine. The current state of the pharmacodynamic model cannot replicate this behavior, and so a threshold was chosen to minimize overall error. Furthermore, it is known that there is interplay between the blood concentration of epinephrine and that of glucose and insulin, but this interaction is not modeled. Another effect of the current pharmacodynamic model is that a higher concentration of epinephrine is needed in the blood to evoke the desired responses for an acute stress action. Additionally, the clearance rate of epinephrine is constant in the engine, whereas there is evidence that the clearance rate of epinephrine changes with blood concentration @cite clutter1980epinephrine. The engine does not currently model the physiological effects of epinephrine deficiency, so a diminished blood concentration of epinephrine will have no effect. Finally, the only physiologic trigger for increased epinephrine release is exercise. Many of the insults modeled are known to cause increased concentrations of epinephrine due to pain or stress, but this release is not directly included in those insult models. Instead, a separate Acute Stress action must be specified to account for this epinephrine release.

The insulin response is currently only dependent on the blood glucose level. In reality, insulin synthesis is dependent on both blood protein and blood glucose concentrations. The current dependence on just blood glucose is a reasonable assumption, because the resting synthesis rate has been accurately validated. For future releases, where meal ingestion will occur, the assumption will also hold true unless a meal consisting of protein occurs.

@anchor endocrine-actions
Actions
-------
### Insults
#### Acute Stress
The Acute Stress action is used to directly modify the production of epinephrine. In the human body, stress or anxiety causes a sympathetic release of epinephrine and norepinephrine from the adrenal medullae. In the engine, an Acute Stress action is rated a severity between 0 and 1, and that severity is used to modify the epinephrine release rate linearly. A severity of 0 will return the rate to the normal, basal level, while a severity of 1 will increase the release rate by a factor of 30. This model was chosen to approximate the physiological responses observed during mild pain, mental stress tests, and panic attacks @cite greisen2001acute @cite herd1991cardiovascular @cite wilkinson1998sympathetic. This does does incorporate the effects of adrenaline on the physiology, which is also released during stress, fear, etc.

Results and Conclusions
=======================

@anchor endrocrine-validation
Validation - Resting Physiologic State
--------------------------------------

In the engine, the hormone epinephrine is the same substance as the synthetic epinephrine available for injection. This generic epinephrine substance was validated as part of the drugs validation detailed in the @ref drugs-validation-pharmacodynamic "Drugs Validation's Table 1". At resting physiological levels, epinephrine has no pharmacodynamic effects.

Validation - Actions
--------------------------------------
@anchor endocrine-acute-stress
### Acute Stress
The effects of epinephrine release on the physiology can be clearly seen by triggering an Acute Stress action. The patient in this scenario undergoes three bouts of Acute Stress, with the first representing mild pain, the second representing mental stress, and the third representing a panic attack. The severity levels were chosen by checking the blood concentration of epinephrine to ensure it met published values. However, a known issue with epinephrine modeling in the current engine release is that higher concentrations of epinephrine are needed to produce the effects noted in literature. In order to achieve the physiological effects, the severity was proportionally scaled up. This shortcoming is the reason for the failing epinephrine concentrations, but it allows for the other effects of epinephrine to be modeled, which has beneficial effects for other systems utilizing epinephrine. Heart rate behaves as expected, but blood pressure effects aren't always in line with expectations. This is likely an effect of baroreceptor reflex counterbalancing epinephrine effects. Figure 2 shows the blood concentration and select effects with the acute stress action.

<center>
<table border="0">
<tr>
    <td><a href="./plots/Endocrine/AcuteStressBloodEpi.jpg"><img src="./plots/Endocrine/AcuteStressBloodEpi.jpg" width="550"></a></td>
    <td><a href="./plots/Endocrine/AcuteStressHR.jpg"><img src="./plots/Endocrine/AcuteStressHR.jpg" width="550"></a></td>
</tr>
<tr>
    <td><a href="./plots/Endocrine/AcuteStressSystolic.jpg"><img src="./plots/Endocrine/AcuteStressSystolic.jpg" width="550"></a></td>
    <td><a href="./plots/Endocrine/AcuteStressDiastolic.jpg"><img src="./plots/Endocrine/AcuteStressDiastolic.jpg" width="550"></a></td>
</tr>
<tr>
    <td colspan="2"><a href="./plots/Endocrine/AcuteStressLegend.jpg"><img src="./plots/Endocrine/AcuteStressLegend.jpg" width="1100"></a></td>
</tr>
</table>
</center>
<center><i>
*Figure 2. Epinephrine is released in response to Acute Stress actions.*
</i>
</center><br>

<center>
*Table 1. Actions associated with the %Endocrine System were validated by comparing the engine output to expected trends and data. Engine results show favorable agreement (green), some agreement (yellow), or bad agreement (red). Results mostly matched expected trends.*
</center>

|	Action	|	Notes	|	Action Occurrence Time (s)	|	Sampled Scenario Time (s)	|	Arterial Epinephrine (ug/L)	|	Systolic Pressure (mmHg)	|	Diastolic Pressure (mmHg)	|	Heart Rate (1/min)	|	Pulmonary Resistance (cmH2O-s/L)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Acute Stress	|	Severity 0.3; mild pain	|	20	|	320	|<span class="success">	90% Increase @cite greisen2001acute Increase @cite herd1991cardiovascular	</span>|<span class="danger">	15 mmHg Increase @cite herd1991cardiovascular	</span>|<span class="danger">	17 mmHg Increase @cite herd1991cardiovascular	</span>|<span class="danger">	Increase @cite herd1991cardiovascular	</span>|<span class="success">	Decrease @cite lewis1985therapy	</span>|
|	Acute Stress	|	Severity 0	|	320	|	620	|<span class="success">	.034 ug/L @cite wortsman1984adrenomedullary	</span>|<span class="success">	100-140 mmHg @cite Leeuwen2015laboratory	</span>|<span class="success">	60-90 mmHg @cite Leeuwen2015laboratory	</span>|<span class="success">	72 @cite guyton2006medical	</span>|<span class="success">	Normal	</span>|
|	Acute Stress	|	Severity 0.6; mental stress	|	620	|	920	|<span class="success">	37-273% Increase @cite herd1991cardiovascular .09 @cite wilkinson1998sympathetic	</span>|<span class="success">	15 mmHg Increase @cite herd1991cardiovascular 143.8 @cite wilkinson1998sympathetic	</span>|<span class="success">	8 mmHg Increase @cite herd1991cardiovascular	</span>|<span class="success">	Increase @cite herd1991cardiovascular  82 @cite wilkinson1998sympathetic	</span>|<span class="success">	Decrease @cite lewis1985therapy	</span>|
|	Acute Stress	|	Severity 0	|	920	|	1220	|<span class="success">	.034 ug/L @cite wortsman1984adrenomedullary	</span>|<span class="success">	100-140 mmHg @cite Leeuwen2015laboratory	</span>|<span class="success">	60-90 mmHg @cite Leeuwen2015laboratory	</span>|<span class="success">	72 @cite guyton2006medical	</span>|<span class="success">	Normal	</span>|
|	Acute Stress	|	Severity 0.9; panic attack	|	1220	|	1520	|<span class="success">	>50-fold increase @cite chendran2019induce, 54-199% Increase @cite wilkinson1998sympathetic	</span>|<span class="success">	24-36 mmHg Increase @cite stratton1985hemodynamic	</span>|<span class="warning">	NC to Slight Increase @cite wilkinson1998sympathetic	</span>|<span class="success">	85-108 @cite wilkinson1998sympathetic	</span>|<span class="success">	Decrease @cite lewis1985therapy	</span>|

@anchor endrocrine-conclusions
Conclusions
-----------

The hormones included in the current %Endocrine System provide a validated response to the effects modeled.

Future Work
===========
@anchor endrocrine-comingsoon
Coming Soon
-----------

Future work will focus on a complete endocrine response to glucose regulation including: detection of glucose levels in the blood stream and release of insulin to appropriately manage those levels. Pathology states of glucose in the patient will be included. We plan to add adrenaline and its link to the acute stress action.

Recommended Improvements
------------------------

An area of potential future advancement is to expand the hormone library, including sex-specific and reproductive hormones.

Appendices
==========

Data Model Implementation
-------------------------

@ref EndocrineSystemTable "Endocrine" 