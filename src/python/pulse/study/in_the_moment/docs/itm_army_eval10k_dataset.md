In The Moment - Army Eval Dataset {#itm_army_eval10k_dataset}
==========================================================

The following images show how our eval dataset compares to the expected statistical distributions.

@htmlonly
<table border="2" align="center"> 
    <tr>
        <td><a href="./Images/itm/army/10k/eval_casualties_age_histogram.jpg">
            <img src="./Images/itm/army/10k/eval_casualties_age_histogram.jpg" alt="" border=3 style="width:400px"></a>
        </td>
        <td><a href="./Images/itm/army/10k/eval_casualties_sex.png">
            <img src="./Images/itm/army/10k/eval_casualties_sex.png" alt="" border=3 style="width:200px"></a>
        </td>
        <td><a href="./Images/itm/army/10k/eval_casualties_statistics.png">
            <img src="./Images/itm/army/10k/eval_casualties_statistics.png" alt="" border=3 style="width:500px"></a>
        </td>
    </tr>
    <tr>
        <th><center>Age</center></th>
        <th><center>Sex</center></th>
        <th><center>Body Measurements</center></th>
    </tr>
</table>
@endhtmlonly
<center>
<i>Casualty Demographics</i>
</center>

<br><br>
@htmlonly
<a href="./Images/itm/army/10k/eval_casualties_injury_statistics.png">
<img src="./Images/itm/army/10k/eval_casualties_injury_statistics.png" alt="" border=3></a>
@endhtmlonly
<center>
<i>Casualty Injuries</i>
</center>

###### Naming convention:

Casualty # (AISi) \[sub_type\] \[anatomic location\]\[Time of Death\]
- The `i` next to the AIS indicates that an intervention could be used to treat an injury, if the vitals indicate to do so.
- Injuries such as `laceration_contusion` can be modelled differently, `the sub_type` indicates which model was used.
- The `anatomic location` specifies which Pulse compartment is affected by this injury.

<a href="./files/itm/army/itm_eval10k_dataset.json">Download the Pulse dataset</a>

<a href="./files/itm/army/itm_eval10k_align.json">Download the Align dataset</a>

#### Head and Neck

##### Airway Obstruction

@insert itm_army_eval10k_head_and_neck_airway_obstruction_table.md

##### Superficial

@insert itm_army_eval10k_head_and_neck_superficial_table.md

##### Traumatic Brain Injury (TBI)

@insert itm_army_eval10k_head_and_neck_tbi_table.md

---

#### Thorax

The thorax can contain more than one injury type (polytrauma). 
Casualties are sorted below into the injury type with the greatest severity.

##### Fracture

@insert itm_army_eval10k_thorax_fracture_table.md

##### Hemorrhage

@insert itm_army_eval10k_thorax_hemorrhage_table.md

##### Hemothorax

@insert itm_army_eval10k_thorax_hemothorax_table.md

##### Pneumothorax

@insert itm_army_eval10k_thorax_pneumothorax_table.md

##### Pulmonary Contusion

@insert itm_army_eval10k_thorax_pulmonary_contusion_table.md

##### Spinal

@insert itm_army_eval10k_thorax_spinal_table.md

---

#### Abdomen

##### Hemorrhage

@insert itm_army_eval10k_abdomen_hemorrhage_table.md

##### Laceration / Contusion

@insert itm_army_eval10k_abdomen_laceration_contusion_table.md

---

#### Extremities

##### Burn / Nerve

@insert itm_army_eval10k_extremity_burn_nerve_table.md

##### Contusion / Sprain / Strain

@insert itm_army_eval10k_extremity_contusion_sprain_strain_table.md

##### Fracture / Dislocation


@insert itm_army_eval10k_extremity_fracture_dislocation_table.md

##### Hemorrhage

@insert itm_army_eval10k_extremity_hemorrhage_table.md


