## Mechanical Ventilator Respiratory Disease Validation

### Introduction

The Pulse Physiology Model is being used as the basis for an interactive educational project to train health care providers in the art and science of mechanical ventilation. The idea is to present the student with a simulated patient and let them respond with appropriate ventilator settings to achieve the three goals of mechanical ventilation: Safety (adequate gas exchange an protection from ventilator induced lung injury), Comfort (maximum synchrony between patient demand for flow and ventilator delivery) and Liberation (minimum duration ventilation through appropriate weaning). Simulated patients fall into three categories: healthy (normal physiology), acute respiratory distress syndrome (ARDS), and chronic obstructive pulmonary disease (COPD). 

Each disease state can have four levels: healthy, mild, moderate, and severe. The severity of ARDS is determined by the PaO2/FiO2 ratio (mild = 200 to ≤ 300, moderate = 100 to ≤ 200, severe ≤ 100). The severity of COPD is based on the % predicted FEV1 (forced expiratory volume in 1 second) as assessed by a pulmonary function test. The % predicted FEV1 can be classified into 4 different levels according to the GOLD standard. 

### Data Sources

No single reference provides enough data to link severity of disease to ventilator settings and expected values for gas exchange and mechanics. Several references were combined for a complete set of simulation parameters. The Validation procedure provides the evidence regarding the suitability of the combinations.

### Validation Procedure

One simulation run was performed for each combination of ventilator settings and disease state.
For each disease state, a tolerance range was defined for observed values of a simulation run for Rinsp, Rexp, Cstat, VD/VT, as +/- 10% of the model target values. 

### Results

@secreflist
  @refitem MechanicalVentilatorHealthy "Healthy Validation"
  @refitem MechanicalVentilatorARDS "ARDS Validation"
  @refitem MechanicalVentilatorCOPD "COPD Validation"
  @refitem MechanicalVentilatorRecruitment "Recruitment Validation"
@endsecreflist
