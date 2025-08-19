In The Moment - Army Example Dataset {#itm_army_example_dataset}
============================================================

The following example dataset provides casualties exercising the bounds of the army distribution data.
Casualties are provided for each applicable AIS score for all injury locations and types.
Several injury types can be applied to different anatomical locations and there are examples using the appropriate Pulse compartment.

Naming convention:

Casualty # (AISi) \[sub_type\] \[anatomic location\]\[Time of Death\]

- The `i` next to the AIS indicates that an intervention could be used to treat an injury, if the vitals indicate to do so.
- Injuries such as `laceration_contusion` can be modelled differently, `the sub_type` indicates which model was used.
- The `anatomic location` specifies which Pulse compartment is affected by this injury.

Each casualty is generated using the Pulse StandardMale patient as the casualty.

<a href="./files/itm/army/itm_example_dataset.json">Download the Pulse dataset</a>

<a href="./files/itm/army/itm_example_align.json">Download the Align dataset</a>

<a href="./files/itm/army/itm_align_example_icl.json">Download the Align ICL dataset</a>

#### Head and Neck

##### Airway Obstruction

@insert itm_army_example_head_and_neck_airway_obstruction_table.md

##### Superficial

@insert itm_army_example_head_and_neck_superficial_table.md

##### Traumatic Brain Injury (TBI)

@insert itm_army_example_head_and_neck_tbi_table.md

---

#### Thorax

The thorax can contain more than one injury type (polytrauma). 
Casualties are sorted below into the injury type with the greatest severity.

##### Fracture

@insert itm_army_example_thorax_fracture_table.md

##### Hemorrhage

@insert itm_army_example_thorax_hemorrhage_table.md

##### Hemothorax

@insert itm_army_example_thorax_hemothorax_table.md

##### Pneumothorax

@insert itm_army_example_thorax_pneumothorax_table.md

##### Pulmonary Contusion

@insert itm_army_example_thorax_pulmonary_contusion_table.md

##### Spinal

@insert itm_army_example_thorax_spinal_table.md

---

#### Abdomen

##### Hemorrhage

@insert itm_army_example_abdomen_hemorrhage_table.md

##### Laceration / Contusion

@insert itm_army_example_abdomen_laceration_contusion_table.md

---

#### Extremities

##### Burn / Nerve

@insert itm_army_example_extremity_burn_nerve_table.md

##### Contusion / Sprain / Strain

@insert itm_army_example_extremity_contusion_sprain_strain_table.md

##### Fracture / Dislocation

@insert itm_army_example_extremity_fracture_dislocation_table.md

##### Hemorrhage

@insert itm_army_example_extremity_hemorrhage_table.md


