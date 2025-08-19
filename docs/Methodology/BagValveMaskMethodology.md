Bag Valve Mask Methodology {#BagValveMaskMethodology}
==========================

@anchor bvm-overview
# Overview

The bag valve mask (also known as Ambu bag or manual resuscitator) model is a generic representation of a hand-held device commonly used to provide positive pressure ventilation to patients who are not breathing or not breathing adequately. 

<center>
<a href="./Images/BagValveMask/BagValveMaskPhoto.jpg"><img src="./Images/BagValveMask/BagValveMaskPhoto.jpg" width="400"></a>
<i>@figuredef {BagValveMaskPhoto} A disposable BVM Resuscitator @cite bagvalvemask.</i>
</center><br>

@anchor bvm-system
# System Design

## Data Flow

### Preprocess

The bag valve mask equipment object modifies circuit and substance values during the preprocess phase.  It sets the connection to the respiratory system based on the airway mode, applies settings, calculates the instantaneous driving pressure value, and sets substance and aerosol values at the source.

### Process

The current implementation has no specific circuit or transport process
functionality for the bag valve mask. Bag valve mask processing
is currently done in the %Respiratory System with the combined circuit
methodology.

### Postprocess

The Postprocess step moves values calculated in the Process step from the next
time step calculation to the current time step calculation. The current implementation has no 
specific post process functionality for the bag valve mask. All postprocessing is done in the
%Respiratory System with the combined circuit methodology.

@anchor bvm-features
## Features and Capabilities

### The Bag Valve Mask Circuit

The bag valve mask model consists of a pressure/flow source to model bag squeezing. Resistances are present between the bag, valve, filter, and connection.  There is a seal resistance that determines the amount of air that escapes during the simulation. @figureref {BagValveMaskCircuit} shows the bag valve mask circuit.  The compartments and transport graph mirrors the circuit.  Substance values are set on the reservoir node/compartment, assuming infinite volume.

<center>
<a href="./Images/BagValveMask/BagValveMaskCircuit.png"><img src="./Images/BagValveMask/BagValveMaskCircuit.png" width="400"></a>
<i>@figuredef {BagValveMaskCircuit} Circuit diagram of the bag valve mask model. The circuit employs a driver source (either pressure or flow, depending on the settings) and resistances.</i>
</center><br>

### Connecting to the Respiratory Circuit

When the bag valve mask is used on a patient, there is a direct
connection that allows air to flow freely between both. In the same
fashion, the bag valve mask and %Respiratory circuits in the engine are
directly connected and allowed to share the same fluid. When in use, both individually defined circuits are combined into a
single circuit that is then used for calculations.

@anchor bvm-settings
### Bag Valve Mask Settings

The bag valve mask can be applied in combination with the intubation action with a mask, oropharyngeal, or nasopharyngeal connection. The model includes the following configuration options:
- Connection: On, Off
- Valve Positive End Expired Pressure: PEEP valve setting for minimum pressure during exhalation
- Fraction Inspired Gases: Sets bag gases, including fraction of inspired oxygen (FiO2)
- Concentration Inspired Aerosols: Sets bag aerosol concentrations, including aerosolized drugs
- Circuit Resistances
	- Bag Resistance: Total resistance of the bag connection
	- Filter Resistance: Total resistance of the filter
  - Valve Resistance: Total resistance of the valve
	- Seal Resistance: Total resistance of the leak
- Circuit Volumes
  - Filter Volume: Total volume of air in the filter
	- Connection Volume: Total volume of air in the mask/tube
	- Valve Volume: Total volume of air in the valve


@anchor bvm-actions
### Bag Valve Mask Actions

The bag valve mask can be driven in three different ways with different parameters as outlined below:

Instantaneous mode: Applies a specific pressure or flow value at the bag until overriden or removed by the user
- Squeeze pressure or flow

Squeeze: Perform one full squeeze cycle
- Squeeze pressure or volume
- Expiratory period
- Inspiratory period

Automated: Continuous application of squeeze cycles until stopped by the user
- Squeeze pressure or volume
- Breath frequency
- Inspiratory-expiratory ratio

@anchor bvm-dependencies
### Dependencies

The bag valve mask interacts with 
the %Respiratory System
through a connection that delivers gases and aerosols into the
%Respiratory System (see @ref RespiratoryMethodology). The two 
systems are connected to each other through a path that connects the airway node of the
 %Respiratory System to the mask node (referred as Connection 
in the circuit diagram) of the bag valve mask. During spontaneous ventilation, the airway node of the
%Respiratory System is connected to the atmosphere via the %Environment System. 
This serves as a ground node for the %Respiratory System. 

When the bag valve mask is connected, a network of combined circuits that
include the elements from both the %Respiratory and bag valve mask systems is
created. When the combined circuit is generated at runtime, the
ground environment node connected to the mouth node of the %Respiratory System is
replaced by the Connection node that represents the mask node, 
becoming one combined circuit.
Apart from such interaction, the bag valve mask is
also responsive to the flow resistances of the %Respiratory System. In
this regard, the bag valve mask driver pressure serves as a positive-pressure source for 
the combined circuit. The bag valve mask is linked to the %Environment 
System that regulates the atmospheric/reference pressure.

@anchor bvm-assumptions
## Assumptions and Limitations

Currently, the bag valve mask uses ideal pressure/flow sources.

@anchor bvm-results
# Results and Conclusions

@anchor bvm-settingsvalidation
## Validation

The bag valve mask settings are fully dynamic.  A scenario that varies the settings in several different combinations on a apneic patient is included with the code base and produces the outputs shown in @figureref {BagValveMaskValidation}.

<center>
<table border="0">
<tr>
  <td><a href="./plots/BagValveMask/BagValveMaskApnea_TotalLungVolume.jpg"><img src="./plots/BagValveMask/BagValveMaskApnea_TotalLungVolume.jpg" width="550"></a></td>
  <td><a href="./plots/BagValveMask/BagValveMaskApnea_TidalVolume.jpg"><img src="./plots/BagValveMask/BagValveMaskApnea_TidalVolume.jpg" width="550"></a></td>
</tr>
<tr>
  <td><a href="./plots/BagValveMask/BagValveMaskOxygenSaturation_RR.jpg"><img src="./plots/BagValveMask/BagValveMaskOxygenSaturation_RR.jpg" width="550"></a></td>
  <td><a href="./plots/BagValveMask/BagValveMaskApnea_Aorta-CarbonDioxide-PartialPressure.jpg"><img src="./plots/BagValveMask/BagValveMaskApnea_Aorta-CarbonDioxide-PartialPressure.jpg" width="550"></a></td>
</tr>
<tr>
  <td colspan="2"><center><a href="./plots/BagValveMask/BagValveMaskApnea_TotalLungVolume_Legend.jpg"><img src="./plots/BagValveMask/BagValveMaskApnea_TotalLungVolume_Legend.jpg" width="1100"></a></center></td>
</tr>
</table>
</center>
<center>
<i>@figuredef {BagValveMaskValidation} These plots show the successful implementation of varying bag valve mask settings with an apneic patient.</i>
</center><br>

<center><br>
@tabledef {BagValveMaskValidation} Validation results for the bag valve mask scenario.
</center>

|	Key	|
|	---	|
|<span class="success">	Good agreement: correct trends or <10% deviation from expected	</span>|
|<span class="warning"> 	Some deviation: correct trend and/or <30% deviation from expected	</span>|
|<span class="danger">	Poor agreement: incorrect trends or >30% deviation from expected	</span>|

|	Segment	|	Action Occurrence Time (s)	|	Sampled Time (s)	|	Respiration Rate (bpm)	|	Tidal Volume (mL)	|	Inspiratory-Expiratory Ratio	|	Oxygen Saturation	|	Arterial Carbon Dioxide Pressure (mmHg)	|
|	---	|	---	|	---	|	---	|	---	|	---	|	---	|	---	|
|	Dyspnea: severity = 1.0 (apnea)	|	30	|	90	|<span class="success">	0 (apnea)	</span>|<span class="success">	0 (apnea)	</span>|<span class="success">	0 (apnea)	</span>|<span class="success">	Decrease	</span>|<span class="success">	Increase	</span>|
|	Oropharyngeal intubation; Automated BVM: Comment: Automate a squeeze pressure to give TV ~ 7 mL/kg(ideal),  BreathFrequency: 12(1/min),  InspiratoryExpiratoryRatio: 0.5,  SqueezePressure: 9.3(cmH2O), SqueezeVolume: NaN	|	90	|	150	|<span class="success">	12 (setting)	</span>|<span class="success">	540 (setting)	</span>|<span class="success">	0.5 (setting)	</span>|<span class="success">	Increase	</span>|<span class="success">	Decrease	</span>|
|	Remove SqueezePressure and apply SqueezeVolume: 0.54(L)	|	150	|	210	|<span class="success">	12 (setting)	</span>|<span class="success">	540 (setting)	</span>|<span class="success">	0.5 (setting)	</span>|<span class="success">	No change	</span>|<span class="success">	No change	</span>|
|	Remove intubation and attach mask; PEEP: 5(cmH2O) BreathFrequency: 12(1/min), InspiratoryExpiratoryRatio: 0.5, SqueezePressure: 12.3(cmH2O), SqueezeVolume: NaN	|	210	|		|<span class="success">	12 (setting)	</span>|<span class="success">	Decrease	</span>|<span class="success">	0.5 (setting)	</span>|<span class="success">	Decrease	</span>|<span class="success">	Increase	</span>|
|	Remove SqueezePressure and apply SqueezeVolume: 0.54(L)	|	270	|	330	|<span class="success">	12 (setting)	</span>|<span class="success">	540 (setting)	</span>|<span class="success">	0.5 (setting)	</span>|<span class="success">	Increase	</span>|<span class="success">	Decrease	</span>|
|	Attach oxygen to the reservoir with FiO2 = 0.3	|	330	|	390	|<span class="success">	12 (setting)	</span>|<span class="success">	540 (setting)	</span>|<span class="success">	0.5 (setting)	</span>|<span class="success">	Increase	</span>|<span class="success">	No change	</span>|

@anchor bvm-conclusion
## Conclusion

While the model is a generic representation of the application of a bag valve mask, inhaled gases, and inhaled agent 
administration, this model represents the behavior of a complex piece of equipment that is 
associated with a difficult
medical specialty. The engine provides a whole-body approach to modeling that 
allows for simulation of this complex field. This system is a strong addition to the engine.

@anchor bvm-improvements
# Future Work

## Coming Soon

Nothing is currently planned.

## Recommended Improvements

The bag could be modeled as a compliance to be closer to reality. 

@anchor bvm-appendices
# Appendices

## Data Model Implementation

@ref BagValveMaskAutomatedTable "BagValveMaskAutomated"

@ref BagValveMaskConfigurationTable "BagValveMaskConfiguration"

@ref BagValveMaskSqueezeTable "BagValveMaskSqueeze"

@ref BagValveMaskInstantaneousTable "BagValveMaskInstantaneous"


## Glossary

FiO2 - Fraction of Inpspired Oxygen

PEEP - Positive End Expired Pressure

SpO2 - Oxygen Saturation
