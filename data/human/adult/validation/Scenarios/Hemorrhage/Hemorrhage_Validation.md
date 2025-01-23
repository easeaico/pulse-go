### Hemorrhage

A hemorrhage is a significant reduction in blood volume, which triggers a physiologic response to stabilize cardiovascular function. Hypovolemia is any loss in blood volume, where a loss of more than 35% is considered hypovolemic shock. Hemorrhage causes a reduction in filling pressure for the circulation, leading to a decrease in venous return. This is evidenced by the decrease in mean arterial pressure and cardiac output. If these physiologic values continue to drop, hemorrhagic or hypovolemic shock will occur. There are three stages of shock: a nonprogressive stage, which the normal circulatory responses will lead to a recovery; a progressive stage, which leads to progressively worsening condition and eventual death without intervention; and an irreversible stage, which leads to death regardless of intervention. The sympathetic response is triggered by the decrease in mean arterial blood pressure, specifically by causing the stretch receptors (baroreceptors) to activate. This response triggers an increase in systemic vascular resistance, heart rate, and a decrease in venous compliance. This is discussed in detail in the @ref NervousMethodology. 

Hemorrhage can be initiated in the engine through two methods. The first method allows the user to characterize the hemorrhage by specifying the location (compartment) and bleed rate. Multiple hemorrhages can be applied to a single compartment or to multiple compartments. The user specifies a cardiovascular compartment to apply a hemorrhage. After the hemorrhage has been specified, the total loss rate is the sum of each individual bleed rate to that compartment. This value is set as a negative flow source. This results in a decrease in total blood volume that is linearly proportional to the total loss rate. This flow rate will remain constant throughout the computation. As the blood volume decreases, the blood flow to each compartment will begin to decrease. This could lead to an invalid flow rate for the compartment over time. A second method for specifying hemorrhage deals with this issue. A hemorrhage can also be characterized by specifying the location (compartment) and a severity. The severity is specified with a value between 0 and 1. A path is added to the cardiovascular circuit, but instead of specifying a negative flow rate, a resistance is specified on the path. This provides a calculated flow rate that will increase and decrease based on the dynamic physics of the circuit. This will prevent the insufficient blood flow/volume errors that can occur if the flow rate is not manually managed. When a hemorrhage is initiated with a severity, a minimum and maximum resistance are calculated to bound the severity, as shown in Equations 3 and 4, respectively.

\f[R_{\min} = (P-P_{T})/cQ \f]
<center>
<i>Equation 3.</i>
</center><br> 

Where R<sub>min</sub> is the minimum resistance, P is the blood pressure at the compartment hemorrhaging, P<sub>T</sub> is the pressure at the hemorrhage flow outlet, Q is the flow through the hemorrhage compartment (not the hemorrhage flow), and c a tuning factor. The tuning factor is employed to ensure a severity of 1.0 corresponds to a hemorrhage rate of approximately 90% of the flow through the compartment. The severity specified in the hemorrhage action is then used to calculate the resistance on the path. 

\f[R_{\max} = (c_{1})*R_{\min}/s \f]
<center>
<i>Equation 4.</i>
</center><br>

Figure 8 demonstrates the different severity specifications and the impact on the hemorrhage flow rate as the severity is changed or the body responds to the hemorrhage. The results show that the hemorrhage severity changes the flow rate for the hemorrhage as expected, i.e., a 0.5 severity corresponds to 50% of the flow associated with a severity of 1.0. The results also show that as time passes the flow rate will naturally decrease without changing the severity to correspond to the reduction in blood pressure that occurs with hemorrhage. These results also demonstrate the ability to transition from a severity to a flow implementation and back to severity, if required. 

@htmlonly
<center>
<table>
<tr>
<td><a href="./plots/Cardiovascular/HemorrhageSeverity.jpg"><img src="./plots/Cardiovascular/HemorrhageSeverity.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/HemorrhageSeverityToFlow.jpg"><img src="./plots/Cardiovascular/HemorrhageSeverityToFlow.jpg" width="550"></a>
</td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>Figure 8. Hemorrhage can be applied as a severity or as a flow rate. A 1.0 severity will be approximately 100% of available flow. The image shows a severity of 1.0, reduced to 0.5, then further to 0.25, and 0. It then returns to 1.0. The flow will change over time as the pressure in the system varies. (Right) Further, the hemorrhage can be changed from a severity to a flow rate. The flow rate will not change when the it is directly specified. (Left)</i>
</center><br>

### Hemorrhage Validation

The hemorrhage response was validated with a comparison to the literature. The mean arterial pressure and cardiac output were computed as a function of their baseline value and plotted with the percent blood loss, as shown in Figure 9. The computed results are shown on the left and the validation data @cite guyton2006medical is shown on the right.

@htmlonly
<center>
<table>
<tr>
<td><a href="./plots/Cardiovascular/MeanArterialPressureShock.jpg"><img src="./plots/Cardiovascular/MeanArterialPressureShock.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/CardiacOutputShock.jpg"><img src="./plots/Cardiovascular/CardiacOutputShock.jpg" width="550"></a>
</td>
</tr>
</table>
<br>
</center>
@endhtmlonly
<center>
<i>Figure 9. Normalized mean arterial pressure (left) and cardiac output (right) as blood loss increases for the Pulse model and the validation data @cite guyton2006medical.</i>
</center><br>

For the hemorrhage to shock scenario, our results maintain MAP through a 20% blood loss and CO begins to slowly decrease as expected. At 20%, we see an approximately linear drop in MAP from a as expected compared to experimental data from @cite guyton2006medical. The cardiac output shows the correct trend but a larger error for this region. The "last ditch" plateau is then exhibited from a blood loss of just under 35% to just under 45%. The MAP and CO then drop precipitously as expected. 

The different types of shock are evident in the data collected for groups of dogs and published in @cite guyton2006medical. Groups I, II, and III show cases of nonprogressive shock, Groups IV, and V show cases of progressive shock, and Group VI is an irreversible shock case. The first three groups recover without intervention, the final case leads quickly to death, and the Group IV and V cases show a short rebound before the physiologic decline that occurs without treatment. These cases were duplicated in the Pulse engine. The results and comparison to validation data are shown in Figure 10.

@htmlonly
<center>
<table>
<tr>
<td><a href="./plots/Cardiovascular/HemorrhageGroups.jpg"><img src="./plots/Cardiovascular/HemorrhageGroups.jpg" width="550"></a>
</td>
<td style="vertical-align:top"><a href="./Images/Cardiovascular/HemorrhageGroupsValidation.jpg"><img src="./Images/Cardiovascular/HemorrhageGroupsValidation.jpg" width="250"></a>
</td>
</tr>
</table>
</br>

</center>
@endhtmlonly
<center>
<i>Figure 10. Normalized mean arterial pressure for different hemorrhage severities to demonstrate the different shock types. The computed Pulse results are on the left and the validation data @cite guyton2006medical is on the right.</i>
</center><br>

For the first three group hemorrhage scenarios (90%, 65%, and 50% blood loss), if the hemorrhage is arrested the MAP begins to rise and reaches a stable value. However, for the remaining three scenarios, the hemorrhage is unrecoverable for the patient. This is expected compared to the experimental data and for the degree of shock. However, one limitation of the model is that at the turning point between progressive and irreversible shock, the expected behavior is a temporary recovery lasting minutes to hours followed by deterioration and death. The current model has no ability to reverse the curve once the final deterioration toward deaths occurs. This is triggered at a blood pressure of approximately 40-45 mmHg. While the outcome is the same, the short recovery is not captured. Future work will incorporate this improvement.

We also saw the expected blood volume, pressure, heart rate, and substance concentration values follow expected trends for the fluid resuscitation scenarios. Figures 11 and 12 show the appropriate substance behavior coupled with the blood volume changes. Like blood volume, the decrease in the substance will be linearly proportional to the bleed rate. For more specific information regarding these substances and their loss due to bleeding, see @ref BloodChemistryMethodology and @ref SubstanceTransportMethodology. Figure 11 shows the blood volume and hemoglobin content before, during, and after a massive hemorrhage event with no intervention other than the cessation of hemorrhage. Figure 12 shows a hemorrhage event with subsequent saline administration. Note that the hemoglobin content remains diminished as the blood volume recovers with IV saline. By comparison, Figure 13 shows a scenarios with blood-product intervention and has the hemoglobin increasing with the blood infusion.

@htmlonly
<center>
<table>
<tr>
<td><a href="./plots/Cardiovascular/Class4NoFluid_BloodVolume.jpg"><img src="./plots/Cardiovascular/Class4NoFluid_BloodVolume.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/Class4NoFluid_Hb.jpg"><img src="./plots/Cardiovascular/Class4NoFluid_Hb.jpg" width="550"></a>
</td>
</tr>
</table>
</br>
<a href="./plots/Cardiovascular/Class4NoFluid_Legend.jpg"><img src="./plots/Cardiovascular/Class4NoFluid_Legend.jpg" width="450"></a>
</center><br>
@endhtmlonly
<center>
<i>Figure 11. Blood volume and hemoglobin content before, during, and after a massive hemorrhage event with no subsequent intervention.</i>
</center>
<br>

@htmlonly
<table>
<tr>
<td><a href="./plots/Cardiovascular/Class2Saline_BloodVolume.jpg"><img src="./plots/Cardiovascular/Class2Saline_BloodVolume.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/Class2Saline_Hb.jpg"><img src="./plots/Cardiovascular/Class2Saline_Hb.jpg" width="550"></a>
</td>
</tr>
</table>
</br>
<a href="./plots/Cardiovascular/Class2Saline_Legend.jpg"><img src="./plots/Cardiovascular/Class2Saline_Legend.jpg" width="450"></a>
</br>
</center>
@endhtmlonly
<center>
<i>Figure 12. Blood volume and hemoglobin content before, during, and after a massive hemorrhage event with a subsequent infusion of saline.</i>
</center><br>

@anchor cardiovascular-blood-administration

@htmlonly
<center>
<table>
<tr>
<td><a href="./plots/Cardiovascular/Class2Blood_BloodVolume.jpg"><img src="./plots/Cardiovascular/Class2Blood_BloodVolume.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/Class2Blood_Diastolic.jpg"><img src="./plots/Cardiovascular/Class2Blood_Diastolic.jpg" width="550"></a>
</td>
</tr>
<tr>
<td><a href="./plots/Cardiovascular/Class2Blood_Hb.jpg"><img src="./plots/Cardiovascular/Class2Blood_Hb.jpg" width="550"></a>
</td>
<td><a href="./plots/Cardiovascular/Class2Blood_Systolic.jpg"><img src="./plots/Cardiovascular/Class2Blood_Systolic.jpg" width="550"></a>
</td>
</tr>
</table>
</br>
<a href="./plots/Cardiovascular/Class2Saline_Legend.jpg"><img src="./plots/Cardiovascular/Class2Saline_Legend.jpg" width="500"></a>
</br>
</center>
@endhtmlonly
<center>
<i>Figure 13. The class 2 hemorrhage scenario shows the blood volume decreasing linearly with the constant 250 milliliter per minute bleeding rate. The blood hemoglobin content follows this exact trend. At the conclusion of the bleed, the blood volume and hemoglobin are at a lower value. Five hundred (500) milliliters of blood is then administered intravenously over the course of 5 minutes. Both the blood volume and hemoglobin content increase linearly with this administration.</i>
</center>

The hemorrhage action is further tested using several scenarios:

@secreflist
  @refitem HemorrhageClass1Femoral "Class 1 Hemorrhage from the Femoral Artery Validation"
  @refitem HemorrhageClass2Brachial "Class 2 Hemorrhage from the Brachial Artery Validation"
  @refitem HemorrhageClass2Blood "Class 2 Hemorrhage with Whole Blood Administration Validation"
  @refitem HemorrhageClass2Saline "Class 2 Hemorrhage with Saline Administration Validation"
  @refitem HemorrhageClass2InternalSpleen "Class 2 Hemorrhage Internally from the Spleen Validation"
  @refitem HemorrhageClass3NoFluid "Class 3 Hemorrhage Validation"
  @refitem HemorrhageClass3PackedRedBloodCells "Class 3 Hemorrhage with Packed Red Blood Cells Administration Validation"
  @refitem HemorrhageClass4NoFluid "Class 4 Hemorrhage Validation"
@endsecreflist

The class 2 hemorrhage scenario with blood intravenous (IV) administration begins with a healthy patient. After a few seconds, a hemorrhage action is initiated at a rate of 250 milliliters (mL) per minute. The hemorrhage continues for four minutes before the bleeding rate is reduced to 0 mL per minute. After two minutes, 500 mL of IV blood is administered intravenously over five minutes. The other hemorrhage scenarios are similar but with different subsequent interventions. There are also two multi-compartment hemorrhage scenarios.

The results show decreases in the systolic pressure and minor increases in the diastolic pressure during the course of the hemorrhage. In response to the decreasing arterial pressures, the baroreceptor response raises the heart rate. The blood volume and hemoglobin content were validated through direct calculation by decreasing blood volume by the bleeding rate multiplied by the time. There is a difference between the computed and simulated blood volume post-hemorrhage due to fluid shift between the intravascular and extravascular space. This shift is evident in the period between cessation of hemorrhage and the start of the infusion (top-left panel of Figure 12).

Following the completion of the hemorrhage, intravenous blood is administered. There will be an increase in hemoglobin content directly proportional to the amount of blood added from the IV. This value was calculated directly from the known blood volume in the IV bag and hemoglobin concentration of the blood.
