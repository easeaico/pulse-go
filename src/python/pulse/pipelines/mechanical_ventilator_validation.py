# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.utils.markdown import table
from pulse.cdm.utils.csv_utils import compute_means
from pulse.cdm.utils.math_utils import generate_percent_difference_span

def write_patient_table(patient_map):
    for patient,values in patient_map.items():
        headings = ["Patient Setting", "Value"]

        data = []
        for header,value in values.items():
            data.append( [header+" ("+value[1]+")", value[0]] )

        fields = [0, 1]

        align = [('^', '<'), ('^', '^')]

        f = open("./test_results/tables/MechanicalVentilator-" + patient + "-Patient-Settings"+".md", "w")
        table(f, data, fields, headings, align)
        f.close()

def write_ventilator_settings_table(settings_map):
    for mode, values in settings_map.items():
        headings = ["Ventilator Setting", "Value"]

        data = []
        for header, value in values.items():
            data.append([header + " (" + value[1] + ")", value[0]])

        fields = [0, 1]

        align = [('^', '<'), ('^', '^')]

        f = open("./test_results/tables/MechanicalVentilator-" + mode + "-Ventilator-Settings" + ".md", "w")
        table(f, data, fields, headings, align)
        f.close()


def write_validation_table(validation_map):
    for key, values in validation_map.items():
        headings = ["Property Name", "Expected Value", "Engine Value", "Percent Error"]

        means = compute_means("./verification/scenarios/miscellaneous/ventilation_mechanics/"+key+"Results.csv",
                              list(values.keys()), 500, 1250)

        data = []
        for i, (header, value) in enumerate(values.items()):
            data.append([header + " (" + value[1] + ")",
                         value[0],
                         f"Mean of {means[i]:.3f}",
                         generate_percent_difference_span(value[0], means[i], epsilon=1e-10, precision=1)])

        fields = [0, 1, 2, 3]

        align = [('<', '<')] * len(headings)

        f = open("./test_results/tables/MechanicalVentilator-" + key + "-Validation" + ".md", "w")
        table(f, data, fields, headings, align)
        f.close()

def data():
    return \
    {
        "patientSettings" : {
            "Healthy" : {
                "Resistance" : [13,"cmH2O-s/L"],
                "Compliance" : [50, "mL/cmH2O"],
                "MusclePressure" : [13, "cmH2O"],
                "SpontaneousRespirationRate" : [15, "bpm"],
                "NeuralInspiratoryTime" : [0.9, "s"]
            },
            "Restrictive" : {
                "Resistance": [20, "cmH2O-s/L"],
                "Compliance": [30, "mL/cmH2O"],
                "MusclePressure": [8, "cmH2O"],
                "SpontaneousRespirationRate": [25, "bpm"],
                "NeuralInspiratoryTime": [0.6, "s"]
            },
            "Obstructive" : {
                "Resistance": [21, "cmH2O-s/L"],
                "Compliance": [53, "mL/cmH2O"],
                "MusclePressure": [14, "cmH2O"],
                "SpontaneousRespirationRate": [18, "bpm"],
                "NeuralInspiratoryTime": [1.0, "s"]
            }
        },
        "ventilatorSettings" :{
            "Healthy_VC_AC": {
                "TidalVolume": [900, "mL"],
                "InspiratoryTime": [1.0, "s"],
                "RespirationRate" : [12, "bpm"],
                "PositiveEndExpiratoryPressure": [5, "cmH2O"],
                "Flow": [60, "L/min"]
            },
            "Healthy_PC_AC": {
                "InspiratoryTime": [1.0, "s"],
                "RespirationRate": [12, "bpm"],
                "PositiveEndExpiratoryPressure": [5, "cmH2O"],
                "PeakInspiratoryPressure": [19, "cmH2O"]
            },
            "Healthy_CPAP": {
                "DeltaPressureSupport": [10, "cmH2O"],
                "PositiveEndExpiratoryPressure": [5, "cmH2O"],
                "Slope": [0.2, "s"]
            },
            "Restrictive_VC_AC": {
                "TidalVolume": [550, "mL"],
                "InspiratoryTime": [1.1, "s"],
                "RespirationRate": [12, "bpm"],
                "PositiveEndExpiratoryPressure": [5, "cmH2O"],
                "Flow": [40, "L/min"]
            },
            "Restrictive_PC_AC": {
                "InspiratoryTime": [1.1, "s"],
                "RespirationRate": [12, "bpm"],
                "PositiveEndExpiratoryPressure": [5, "cmH2O"],
                "PeakInspiratoryPressure": [23, "cmH2O"]
            },
            "Restrictive_CPAP": {
                "DeltaPressureSupport": [10, "cmH2O"],
                "PositiveEndExpiratoryPressure": [5, "cmH2O"],
                "Slope": [0.2, "s"]
            },
            "Obstructive_VC_AC": {
                "TidalVolume": [500,"mL"],
                "InspiratoryTime": [1.1,"s"],
                "RespirationRate": [12,"/min"],
                "PositiveEndExpiratoryPressure": [5,"cmH2O"],
                "Flow": [40,"L/min"]
            },
            "Obstructive_PC_AC": {
                "InspiratoryTime": [1.2,"s"],
                "RespirationRate": [12,"bpm"],
                "PositiveEndExpiratoryPressure": [5,"cmH2O"],
                "PeakInspiratoryPressure": [12,"cmH2O"]
            },
            "Obstructive_CPAP": {
                "DeltaPressureSupport": [10,"cmH2O"],
                "PositiveEndExpiratoryPressure": [5,"cmH2O"],
                "Slope": [0.2,"s"]
            }
        },
        "validationData" : {
            "Healthy_VC_AC": {
                "MechanicalVentilator-RespirationRate(1/min)" : [15, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [7.9, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)" : [900, "mL"]
            },
            "Healthy_PC_AC": {
                "MechanicalVentilator-RespirationRate(1/min)": [15, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [8.6, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)": [1047, "mL"]
            },
            "Healthy_CPAP": {
                "MechanicalVentilator-RespirationRate(1/min)": [15, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [6.9, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)": [905, "mL"]
            },
            "Restrictive_VC_AC": {
                "MechanicalVentilator-RespirationRate(1/min)": [25, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [13, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)": [550, "mL"]
            },
            "Restrictive_PC_AC": {
                "MechanicalVentilator-RespirationRate(1/min)": [25, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [12, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)": [528, "mL"]
            },
            "Restrictive_CPAP": {
                "MechanicalVentilator-RespirationRate(1/min)": [25, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [10, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)": [387, "mL"]
            },
            "Obstructive_VC_AC": {
                "MechanicalVentilator-RespirationRate(1/min)": [18, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [7.3, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)": [493, "mL"]
            },
            "Obstructive_PC_AC": {
                "MechanicalVentilator-RespirationRate(1/min)": [18, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [7.8, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)": [571, "mL"]
            },
            "Obstructive_CPAP": {
                "MechanicalVentilator-RespirationRate(1/min)": [18, "bpm"],
                "MechanicalVentilator-MeanAirwayPressure(cmH2O)": [8.5, "cmH2O"],
                "MechanicalVentilator-TidalVolume(mL)": [657, "mL"]
            }
        }
    }

if __name__ == "__main__":
    baseline = data()
    write_patient_table(baseline["patientSettings"])
    write_ventilator_settings_table(baseline["ventilatorSettings"])
    write_validation_table(baseline["validationData"])
