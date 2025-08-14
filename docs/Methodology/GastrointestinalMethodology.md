%Gastrointestinal Methodology {#GastrointestinalMethodology}
===========================

@anchor GI-overview
Overview
========

Abstract
--------

The %Gastrointestinal System models the ingestion of water and macronutrients and their subsequent digestion and transportation into the %Cardiovascular system. 
Digestion rates for each macronutrient can be specified along with the amount, and if no rates are supplied, a default configurable rate will be used.
The macronutrients are then digested by converting the macronutrient into a specific substance from the stomach and incrementing the amount of that substance into the small intestine's vasculature each timestep. 
Water is digested from the stomach into a gut/chyme compartment and aborbed into the small intestine vasculature at a standard rate.

@anchor GI-introduction
Introduction
------------

### Gastrointestinal Physiology

The %Gastrointestinal tract is responsible for consuming and digesting food, absorbing nutrients, and expelling waste.
The tract consists of the stomach and intestines breaking down food into usable nutrients for the body. 
After being eaten, food is stored in the stomach, gradually digested, then released into the gut chyme. 
At this point, nutrients are either quickly absorbed into the blood through the intestinal wall or further broken down prior to absorption. 
An overview of the digestive tract is shown in @figureref {DigestiveTract}.

<center>
<a href="./Images/GI/DigestiveTractDetail.png"><img src="./Images/GI/DigestiveTractDetail.png"></a>

<i>
@figuredef {DigestiveTract} The human digestive tract is composed of several distinct sections @cite LadyofHats2006Digestive. 
Currently, the gastrointestinal model replicates the behavior of the stomach and small intestine.
</i>
</center><br>

@anchor GI-system
System Design
=============

@anchor GI-background
Background and Scope
--------------------
The %Gastorintestinal System was created so that end users could supply and replenish substances in the body associated with exercise and clearance.
It provides a rudimentary methodology for getting substances into the body that can be expanded upon in the future.

@anchor GI-dataflow
Data Flow
---------

### Reset

The stomach is initialized with configurable amounts of each macronutrient, sodium, calcium, and water.
The stomach contents may be overriden via the Pulse configuration object or via the meal consumption condition
### Preprocess

#### Digestion

The amount of macronutrient to decrement from the stomach is based on the nutrient's digestion rate and the engine timestep.
The decremented amount is then multiplied by a configurable absorption amount to calculate the mass of the nutrient absorbed into the body.
This calculated mass is then used to increment a specific substance into the small intestine vascular compartment.

Nutrient data is converted to substance data in the following manner :
  - Carbohydrates - A transfer mass is directly calculated from the stomach carbohydrate digestion rate and   engine timestep.
                    That mass is then decremented from the stomach carbohydrate mass.
                    The mass is also multiplied by a configurable carbohydrate absorption factor, 
                    and the amount of glucose in the small intestine vasculatur is incremented by that product.
  - Fats - A transfer mass is directly calculated from the stomach fat digestion rate and engine timestep.
           That mass is then decremented from the stomach fat mass.
           The mass is also multiplied by a configurable fat absorption factor, 
           and the amount of tristearin in the small intestine vasculatur is incremented by that product.
  - Protein - A transfer mass is directly calculated from the stomach protein digestion rate and engine timestep.
              That mass is then decremented from the stomach protein mass.
              The mass is also multiplied by a configurable ProteinToUrea factor, 
              and the amount of urea in the small intestine vasculatur is incremented by that product.
  - Calcium - A transfer mass is directly calculated from the configurable stomach calcium digestion rate and engine timestep.
              That mass is then decremented from the stomach calcium mass.
              The mass is also multiplied by a configurable calcium absorption factor, 
              and the amount of calcium in the small intestine vasculatur is incremented by that product.
  - Sodium - A transfer mass is calculated as a function of the concentration of sodium in the stomach and the water digestion rate.
               This mass is decremented from the stomach sodium mass.
               The resulting sodium mass is incremented in the small intestine vasculatur compartment.
  - Water -  A transfer volume is directly calculated from the configurable water digestion rate and engine timestep.
             This volume is decremented from the stomach water volume and the volume is incremented by this transferred volume in the gut chyme compartment.
 
#### Absorption

A static absorption flow rate of 3.3 mL/min is used to manually move water volume and substances from the gut chyme into small intestine vasculature.

### Conditions

#### ConsumeMeal

The ConsumeMeal condition is still under development and will provide a nutrition object along with an elapsed time since those nutrients were in the stomach.
The GI System will then calculate the digested amounts based on the provided time and update the cardiovascular and tissue system accordingly.

### Actions

#### ConsumeNutrients
Each nutrient is optional in the ConsumeNutrients action.
If a nutrient mass is provided with no accompanying digestion rate, the engine will default to the rate specified in the engine configuration. 
If a digestion rate is provided for a macronutrient, it will be combined with the current stomach digestion rate for that macronutrient by volume weighting the two rates together.

### Process
There is no system specific function for Process in %Gastrointestinal.

### Post Process
There is no system specific function for Post Process in %Gastrointestinal.

### Assessments
There are no system specific assessments in %Gastrointestinal.

@anchor GI-features
Features, Capabilities, and Dependencies 
----------------------
### Circuit

<center>
@anchor GI-circuit
<a href="./Images/GI/GICircuit.png"><img src="./Images/GI/GICircuit.png"></a>
<i>
@figuredef {GICircuit} The %GI circuit is made up of nodes and paths with elements and is connected to extravascular tissue and the cardiovascular system.
</i>
</center><br>

There is a steady secretion of fluid, enzymes, and other substances into the various compartments of the digestive tract.
The constant secretion of fluid into the digestive tract allows for macronutrients to be slowly digested and eventually absorbed through the intestinal wall @cite hall2011guyton.
In the engine, these macronutrients are placed into the CV system through the an absorption rate that mirrors that of the human GI tract.
A digested amount is calculated individually for each substance, and that calculated mass is decremented from the stomach, multiplied by its associated absorption factor and the result is added to the CV system's small intestine 1 node @ref GI-circuit.
Water, on the other hand, is digested from the stomach to the small instestine C1 node. A flow source then transports the water volume to the CV system as part of our circuit calculation.

The digestion rate of macronutrients is heavily dependent on the nature of the source food, as the digestive tract needs to free substances from the rest of the food's molecular structure before it can be absorbed @cite wolever1991glycemic.
Fats spend several hours being emulsified by the small intestine and are then quickly absorbed once exposed to pancreatic enzymes @cite hall2011guyton. 
Due to the current lack of a hepatic model, the %Gastrointestinal System breaks protein into urea, simulating the deamination of amino acids in the liver @cite hall2011guyton @cite bean1984regulation @cite haussinger1991hepatocyte. 
Many of the small ions like sodium are quickly absorbed with active transport in a manner similar to the %Renal system @cite hall2011guyton.
An exception to this is calcium, which is carefully regulated by parathyroid hormone to ensure that the body's requirements are met @cite hall2011guyton.
Water is absorbed via the osmotic gradient that either naturally exists between the gut chyme and the blood or that is created by the active transport of substances across the intestinal wall @cite hall2011guyton.
<center>
| Macronutrient | Resultant Substance |                       Digestion Rate (R)                         | Absorption Fraction (A<sub>frac</sub>) | Gut Substance Mass(M<sub>g</sub>) Stomach Nutrient Mass (M<sub>s</sub>)     |
|:---:          |:---:                |:---:                                                             |:---:                                   |:---:                                                                        |
| Carbohydrate  | Glucose             | 0.5 - 0.625 g/min @cite hall2011guyton @cite wolever1991glycemic | 0.800 @cite hall2011guyton             | M<sub>g</sub> = R*&Delta;T*A<sub>frac</sub>                                 |
| Fat           | Tristearin          | 0.055 g/min @cite hall2011guyton                                 | 0.248 @cite hall2011guyton             | M<sub>g</sub> = R*&Delta;T*A<sub>frac</sub>                                 |
| Protein       | Urea                | 0.071 - 0.157 g/min @cite dangin2001digestion                    | 0.405 @cite hall2011guyton             | M<sub>g</sub> = R*&Delta;T*A<sub>frac</sub>                                 |
| Calcium       | Calcium             | 2.7 mg/min @cite gallagher1979intestinal                         | 0.25  @cite haussinger1990nitrogen     | M<sub>g</sub> = R*&Delta;T*A<sub>frac</sub>                                 |
| Sodium        | Sodium              | N/A                                                              | 1.0                                    | M<sub>g</sub> = M<sub>s</sub>/V<sub>H2O</sub><i>R<sub>H2O</sub></i>&Delta;T |
| Water         | Water               | 0.417 mL/s @cite Peronnet2012Pharmacokinetic                     | 1.0                                    | V<sub>H2O</sub> = R*&Delta;T                                                |

*@tabledef {GIMacronutrientConversion} How macronutrients are converted into substances.*
</center>

Water is transported into the cardiovascular circuit at a maximum flow rate of 3.3mL/s.

@anchor GI-dependencies
Dependencies
------------
The %Gastrointestinal System is not dependent on any other system, but other systems are dependent on GI to provided and replenish substances that they use.

@anchor GI-assumptions
Assumptions and Limitations
---------------------------

The current digestion methodology is rudimentary and makes very general assumptions for the macronutrient types and assumes all digestive rates are constant when digesting from the stomach to the vasculature.
Currently there is no modeling of feces in the lower portion of the digestive tract. The calculated unabsorbed substance masses are simpley removed from the system.
Active transport is used across cell walls with no diffusion currently implemented. Contraction of the small intestines to allow for transport and defecation is also not modeled.

@anchor GI-results
Results and Conclusions
=======================
Validation - Resting Physiologic State
--------------------------------------

The %Gastrointestinal System was validated qualitatively by observing the relative changes in the substance and nutrient masses between the stomach contents,
the gut volumes and small intestine volume and substance masses. The transport to the CV system was then shown by demonstrating an increase in substance concentration in the blood.
Concentrations are not solely a function of GI; other systems are potentially using substances in the blood.

<center>
*@tabledef {GIMacronutrientMass} Macronutrient mass in the stomach, and blood as a function of time.*

|Macronutrient/Substance|                                    Stomach Mass(g) / Volume(mL)  vs. Time(s)                                         |                               Blood Concentration (ug/mL) / Volume (mL) vs. Time (s)                                   |
|:---:                  |:---:                                                                                                                 |:---:                                                                                                                   |
|Carbohydrate/Glucose   |<a href="./plots/GI/6hr_StomachCarbs.jpg"><img src="./plots/GI/6hr_StomachCarbs.jpg" height="200" width="400"></a>    |<a href="./plots/GI/6hr_BloodGlucose.jpg"><img src="./plots/GI/6hr_BloodGlucose.jpg" height="200" width="400"></a>      |
|Fat/Tristearin         |<a href="./plots/GI/6hr_StomachFat.jpg"><img src="./plots/GI/6hr_StomachFat.jpg" height="200" width="400"></a>        |<a href="./plots/GI/6hr_BloodTristearin.jpg"><img src="./plots/GI/6hr_BloodTristearin.jpg" height="200" width="400"></a>|
|Protein/Urea           |<a href="./plots/GI/6hr_StomachProtein.jpg"><img src="./plots/GI/6hr_StomachProtein.jpg" height="200" width="400"></a>|<a href="./plots/GI/6hr_BloodUrea.jpg"><img src="./plots/GI/6hr_BloodUrea.jpg" height="200" width="400"></a>            |
|Calcium                |<a href="./plots/GI/6hr_StomachCalcium.jpg"><img src="./plots/GI/6hr_StomachCalcium.jpg" height="200" width="400"></a>|<a href="./plots/GI/6hr_BloodCalcium.jpg"><img src="./plots/GI/6hr_BloodCalcium.jpg" height="200" width="400"></a>      |
|Sodium                 |<a href="./plots/GI/6hr_StomachSodium.jpg"><img src="./plots/GI/6hr_StomachSodium.jpg" height="200" width="400"></a>  |<a href="./plots/GI/6hr_BloodSodium.jpg"><img src="./plots/GI/6hr_BloodSodium.jpg" height="200" width="400"></a>        |
|Water                  |<a href="./plots/GI/6hr_StomachWater.jpg"><img src="./plots/GI/6hr_StomachWater.jpg" height="200" width="400"></a>    |<a href="./plots/GI/6hr_BV.jpg"><img src="./plots/GI/6hr_BV.jpg" height="200" width="400"></a>                          |
</center>

As seen in @tableref {GIMacronutrientMass}, the macronutrient masses in the stomach are depleted based on different digestion rates and eventually reach zero. 
Concentrations are provided on the right column demonstrating the effect of the increasing mass on the blood concentrations.
These overall trends meet the expectations of the model performance.

@anchor GI-conclusions
Conclusions
-----------

The %Gastrointestinal System lays a foundation for future efforts in modeling digestive physiology. The flexible definition of food allows for easy extensibility
of the system by creating the ability for users to add to a library of meals. 

@anchor GI-futurework
Future Work
===========

Coming Soon
-----------
The osmolarity of the digested material is not currently measured. Implementing this would allow for the creation of a more realistic osmolar gradient to determine the water absorption rate and gastric secretion. Permeability of the tissue matrix and substance concentration levels will be added to allow for actual diffusion of substances and fluid instead of active transport being implemented currently.

@anchor GI-recommended
Recommended Improvements
------------------------

- More %Substances: By expanding the list of %substances used in the engine, the %Gastrointestinal model will inherently gain fidelity as mechanisms
  are constructed to properly handle new inputs.

- Defecation: The %Gastrointestinal system should eventually be able to handle undigestible material and the fluid loss due to defecation. This
  will also allow for a variety of disease states to be implemented.
  
- Vomiting: This can be added either as a symptom or an intervention to aid someone who has ingested a harmful substance.

@anchor GI-appendices
Appendices
==========

Glossary
--------

GI - %Gastrointestinal

CV - %Cardiovascular


Data Model Implementation
-------------------------

@ref CardiovascularSystemTable "Cardiovascular"

@ref GastrointestinalSystemTable "Gastrointestinal"

Compartments
------------

The GI system is integrated into the %cardiovascular system, please refer to the compartement description associated with @ref cardiovascular-appendices "Cardiovascular Methodology".