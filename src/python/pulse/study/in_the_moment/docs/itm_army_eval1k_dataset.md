In The Moment - Army Eval Dataset {#itm_army_eval1k_dataset}
===========================================================

The following images show how our eval dataset compares to the expected statistical distributions.

@htmlonly
<table border="2" align="center"> 
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_age_histogram.jpg">
            <img src="./Images/itm/army/1k/eval_casualties_age_histogram.jpg" alt="" border=3 style="width:400px"></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_sex.png">
            <img src="./Images/itm/army/1k/eval_casualties_sex.png" alt="" border=3 style="width:200px"></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_statistics.png">
            <img src="./Images/itm/army/1k/eval_casualties_statistics.png" alt="" border=3 style="width:500px"></a>
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
<a href="./Images/itm/army/1k/eval_casualties_injury_statistics.png">
<img src="./Images/itm/army/1k/eval_casualties_injury_statistics.png" alt="" border=3></a>
@endhtmlonly
<center>
<i>Casualty Injuries</i>
</center>

###### Naming convention:

Casualty # (AISi) \[sub_type\] \[anatomic location\]\[Time of Death\]
- The `i` next to the AIS indicates that an intervention could be used to treat an injury, if the vitals indicate to do so.
- Injuries such as `laceration_contusion` can be modelled differently, `the sub_type` indicates which model was used.
- The `anatomic location` specifies which Pulse compartment is affected by this injury.

<a href="./files/itm/army/itm_eval1k_dataset.json">Download the Pulse dataset</a>

<a href="./files/itm/army/itm_eval1k_align.json">Download the Align dataset</a>

### Tagging Analysis

<br>
@htmlonly
<table border="2" align="center"> 
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_overall_survivability.png">
            <img src="./Images/itm/army/1k/eval_casualties_overall_survivability.png" alt="" border=3></a>
        </td>
    </tr>
</table>
@endhtmlonly
<center>
<i>Overall Survivability</i>
</center>

<br>
@htmlonly
<table border="2" align="center"> 
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais-1.0_survivability.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais-1.0_survivability.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais-2.0_survivability.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais-2.0_survivability.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais-3.0_survivability.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais-3.0_survivability.png" alt="" border=3></a>
        </td>
    </tr>
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais-4.0_survivability.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais-4.0_survivability.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais-5.0_survivability.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais-5.0_survivability.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais-6.0_survivability.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais-6.0_survivability.png" alt="" border=3></a>
        </td>
    </tr>
</table>
@endhtmlonly
<center>
<i>AIS Survivability</i>
</center>

<br>
@htmlonly
<table border="2" align="center"> 
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_start_location_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_start_location_sankey.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_salt_location_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_salt_location_sankey.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_bcd_sieve_location_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_bcd_sieve_location_sankey.png" alt="" border=3></a>
        </td>
    </tr>
</table>
@endhtmlonly
<center>
<i>Location Tag Counts</i>
</center>

<br>
@htmlonly
<table border="2" align="center">
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_start_overall_ais_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_start_overall_ais_sankey.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_salt_overall_ais_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_salt_overall_ais_sankey.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_bcd_sieve_overall_ais_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_bcd_sieve_overall_ais_sankey.png" alt="" border=3></a>
        </td>
    </tr>
</table>
@endhtmlonly
<center>
<i>AIS Tag Counts</i>
</center>

<br>
@htmlonly
<table border="2" align="center"> 
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_start_hemorrhage_ais_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_start_hemorrhage_ais_sankey.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_salt_hemorrhage_ais_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_salt_hemorrhage_ais_sankey.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_bcd_sieve_hemorrhage_ais_sankey.png">
            <img src="./Images/itm/army/1k/eval_casualties_bcd_sieve_hemorrhage_ais_sankey.png" alt="" border=3></a>
        </td>
    </tr>
</table>
@endhtmlonly
<center>
<i>Hemorrhage Tag Counts</i>
</center>

<br>
@htmlonly
<table border="2" align="center"> 
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_overall_tag_counts.png">
            <img src="./Images/itm/army/1k/eval_casualties_overall_tag_counts.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_tag_counts.png">
            <img src="./Images/itm/army/1k/eval_casualties_tag_counts.png" alt="" border=3 style="width:400px"></a>
        </td>
    </tr>
</table>
@endhtmlonly
<center>
<i>Overall Tag Counts</i>
</center>

<br>
@htmlonly
<table border="2" align="center"> 
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais_1.0_tag_counts.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais_1.0_tag_counts.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais_2.0_tag_counts.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais_2.0_tag_counts.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais_3.0_tag_counts.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais_3.0_tag_counts.png" alt="" border=3></a>
        </td>
    </tr>
    <tr>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais_4.0_tag_counts.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais_4.0_tag_counts.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais_5.0_tag_counts.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais_5.0_tag_counts.png" alt="" border=3></a>
        </td>
        <td><a href="./Images/itm/army/1k/eval_casualties_ais_6.0_tag_counts.png">
            <img src="./Images/itm/army/1k/eval_casualties_ais_6.0_tag_counts.png" alt="" border=3></a>
        </td>
    </tr>
</table>
@endhtmlonly
<center>
<i>AIS Tag Counts</i>
</center>

#### Head and Neck

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_head_and_neck_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_head_and_neck_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

##### Airway Obstruction

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_head_and_neck-airway_obstruction_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_head_and_neck-airway_obstruction_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_head_and_neck_airway_obstruction_table.md

##### Superficial

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_head_and_neck-superficial_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_head_and_neck-superficial_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_head_and_neck_superficial_table.md

##### Traumatic Brain Injury (TBI)

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_head_and_neck-tbi_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_head_and_neck-tbi_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_head_and_neck_tbi_table.md

---

#### Thorax

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_thorax_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_thorax_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

The thorax can contain more than one injury type (polytrauma). 
Casualties are sorted below into the injury type with the greatest severity.

##### Fracture

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_thorax-fracture_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_thorax-fracture_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_thorax_fracture_table.md

##### Hemorrhage

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_thorax-hemorrhage_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_thorax-hemorrhage_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_thorax_hemorrhage_table.md

##### Hemothorax

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_thorax-hemothorax_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_thorax-hemothorax_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_thorax_hemothorax_table.md

##### Pneumothorax

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_thorax-pneumothorax_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_thorax-pneumothorax_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_thorax_pneumothorax_table.md

##### Pulmonary Contusion

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_thorax-pulmonary_contusion_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_thorax-pulmonary_contusion_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_thorax_pulmonary_contusion_table.md

##### Spinal

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_thorax-spinal_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_thorax-spinal_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_thorax_spinal_table.md

---

#### Abdomen

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_abdomen_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_abdomen_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

##### Hemorrhage

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_abdomen-hemorrhage_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_abdomen-hemorrhage_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_abdomen_hemorrhage_table.md

##### Laceration / Contusion

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_abdomen-laceration_contusion_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_abdomen-laceration_contusion_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_abdomen_laceration_contusion_table.md

---

#### Extremities

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_extremity_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_extremity_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

##### Burn / Nerve

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_extremity-burn_nerve_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_extremity-burn_nerve_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_extremity_burn_nerve_table.md

##### Contusion / Sprain / Strain

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_extremity-contusion_sprain_strain_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_extremity-contusion_sprain_strain_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_extremity_contusion_sprain_strain_table.md

##### Fracture / Dislocation

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_extremity-fracture_dislocation_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_extremity-fracture_dislocation_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_extremity_fracture_dislocation_table.md

##### Hemorrhage

@htmlonly
<table border="2" ><tr><td>
<a href="./Images/itm/army/1k/eval_casualties_extremity-hemorrhage_survivability.png">
<img src="./Images/itm/army/1k/eval_casualties_extremity-hemorrhage_survivability.png" alt="" border=3 style="width:400px" align="left"></a>
</td></tr></table>
@endhtmlonly

@insert itm_army_eval1k_extremity_hemorrhage_table.md


