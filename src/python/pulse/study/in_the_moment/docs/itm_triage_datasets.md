In The Moment - Triage Datasets {#itm_datasets}
===============================================

#### Army Demographics and Injury Dataset 

This data was used to create a synthetic population in order to analyze the effectiveness of the START, SALT, and BCD Sieve triaging protocols.
Along with this analysis, we also performed a study using the ALIGN system to evaluate the effectiveness for LLMs to triage injuries using these protocols.

@htmlonly
<table border="2" align="center"> 
    <tr>
        <td style="width:50%"><a href="./files/itm/army/Triage_Evaluation.pdf">
            <img src="./files/itm/army/Triage_Evaluation.png" alt="" border=3></a>
        </td>
        <td style="width:50%"><a href="./files/itm/army/ALIGN_Triage.pdf">
            <img src="./files/itm/army/ALIGN_Triage.png" alt="" border=3></a>
        </td>
    </tr>
    <tr>
        <td><center><b>Evaluation of Triage Tagging Protocols Using a Synthetic Dataset Representative of Battlefield Injury Profiles</b></center></td>
        <td><center><b>Aligning a Large Language Model Protocol-Based Medical Triage Decision Making</b></center></td>
    </tr>
</table>
@endhtmlonly
<br>
The Casualty Evaluation Dataset was created to meet the following army distribution requirements.

@insert army_population_table.md

<center>
<a href="./Images/itm/army/age_histogram.png">
<img src="./Images/itm/army/age_histogram.png" alt="" border=3 style="width:200px"></a>
<i>Normalized Age Distributions @cite Demographics2022</i>
</center>
<br>

Injury severities use the AIS (Abbreviated Injury Scale) severity score.
AIS values are: 
- 1.0: Minor
- 2.0: Moderate
- 3.0: Serious
- 4.0: Severe
- 5.0: Critical
- 6.0: Fatal

@insert army_injury_table.md

@secreflist
	@refitem itm_army_example_dataset "Example Dataset"
	@refitem itm_army_eval1k_dataset "1000 Casualty Evaluation Dataset"
	@refitem itm_army_eval10k_dataset "10000 Casualty Evaluation Dataset"
@endsecreflist

<br>

##### Disclaimer

This research was performed in connection with the U.S. Army Contracting Command - Aberdeen
Proving Ground (ACC-APG) and the Defense Advanced Research Projects Agency (DARPA) under contract number W912CG- 24-C-0011.
The views and conclusions are those of the authors and should not be interpreted as presenting the official
policies or position, either expressed or implied, of ACC-APG, DARPA, or the U.S. Government. The U.S. Government is authorized to
reproduce and distribute reprints for Government purposes notwithstanding any copyright notation hereon.
