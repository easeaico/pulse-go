Scenario Files {#ScenarioFile} 
==================

The engine can be used to simulate various physiological scenarios.
The Common Data Model (CDM) provides a Scenario structure that can contain a set of instructions that can be used to drive the engine. 
Below you can see how the content of a JSON scenario file is structured along with examples of all the actions, conditions, assessments supported

A Scenario is a 'canned' instruction set with requested data to be output in a comma delimited file that is executed by the engine and will produce the same results data.
'FATAL' is used below to note boundary cases that will result in a fatal exception, stopping the engine.

If you would like execute scenarios, the CDM contains a class, @ref SEScenarioExec, that can execute any scenario with a physiology engine that implements the PhysiologyEngine interface.
The @ref Toolkit, also provides example scenario files and can also execute scenario files.

An example of a basic scenario file is shown below.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{
  "Name": "Vitals Monitor",
  "Description": "Data associated with a vitals monitor.",
  "PatientConfiguration": { "PatientFile": "StandardMale.json" },
  "DataRequestManager": {
    "DataRequest": [
      
      {"DecimalFormat":{"Precision":2}, "Category":"Physiology",  "PropertyName":"HeartRate",                         "Unit":"1/min"},
      {"DecimalFormat":{"Precision":1}, "Category":"Physiology",  "PropertyName":"ArterialPressure",                  "Unit":"mmHg"},
      {"DecimalFormat":{"Precision":1}, "Category":"Physiology",  "PropertyName":"MeanArterialPressure",              "Unit":"mmHg"},
      {"DecimalFormat":{}             , "Category":"Physiology",  "PropertyName":"SystolicArterialPressure",          "Unit":"mmHg"},
      {"DecimalFormat":{"Precision":1}, "Category":"Physiology",  "PropertyName":"DiastolicArterialPressure",         "Unit":"mmHg"},
      {"DecimalFormat":{"Precision":3}, "Category":"Physiology",  "PropertyName":"OxygenSaturation",                  "Unit":"unitless"},
      {"DecimalFormat":{"Precision":4}, "Category":"Physiology",  "PropertyName":"EndTidalCarbonDioxideFraction",     "Unit":"unitless"},
      {"DecimalFormat":{"Precision":2}, "Category":"Physiology",  "PropertyName":"RespirationRate",                   "Unit":"1/min"},
      {"DecimalFormat":{"Precision":1}, "Category":"Physiology",  "PropertyName":"SkinTemperature",                   "Unit":"degC"},
      
      {"DecimalFormat":{}             , "Category":"GasCompartment", "CompartmentName":"Carina", "SubstanceName":"Oxygen", "PropertyName":"PartialPressure", "Unit":"cmH2O"},
    ]},
    
  "AnyAction": [{
    "AdvanceTime": {
      "Time": {
        "ScalarTime": {
          "Value": 5.0,
          "Unit": "min"
        }
      }
    }
  }]
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The scenario allows for a name and description, but this is not used in execution.
The scenario contains the following execution information:
- Patient File and optional conditions OR An engine state file to start at
- A list of values to return from the engine
- A list of actions to execute over the course of the run

- - -

### Engine state to use

Skip any stabilization and start the engine at any point in time from a previous run.
There are initial states (just after it is stable) for every patient in the bin/states folder

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"Name": "Vitals Monitor",
"Description": "Data associated with a vitals monitor.",
"EngineStateFile":"./states/StandardMale@0s.pbb",

# Or specify a json state file 

"Name": "Vitals Monitor",
"Description": "Data associated with a vitals monitor.",
"EngineStateFile":"./states/StandardMale@0s.json",

# Next comes data requests (See Further Below)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Patient File and optional conditions 

While it is recommended to use an Engine State when running a scenario, you do have the option to initialize the engine with a Patient File and optional conditions.
The specified patient file refers to a file containing @ref Patient_PatientData information.
Replace the EngineStateFile section with a Patient Configuration like this:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"Name": "Condition Example",
"Description": "Data associated with a condition.",
"PatientConfiguration": {
    "PatientFile": "StandardMale.json",
    "Conditions": { "AnyCondition": [{
      "PatientCondition": {
        "ChronicObstructivePulmonaryDisease": {
          "BronchitisSeverity": {
            "Scalar0To1": {
              "Value": 0.7
            }
          },
          "EmphysemaSeverity": {
            "Scalar0To1": {
              "Value": 0.5
            }
          }
        }
      }
    }]}

# Next comes data requests (See Further Below)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Patient Conditions

Conditions give instructions to the engine to apply certain changes to the engine to simulate the specified conditions.
The following are links to the Condition class specification along with XML examples of conditions that can be used in making your own scenarios.

##### Acute Respiratory Distress Syndrome
@copybrief AcuteRespiratoryDistressSyndromeData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "AcuteRespiratoryDistressSyndrome": {
      "Severity": [ 
      { "Compartment": "LeftLung", "Severity": { "Scalar0To1": { "Value": 0.3 }}},
      { "Compartment": "RightLung", "Severity": { "Scalar0To1": { "Value": 0.6 }}}]
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Chronic Anemia
@copybrief ChronicAnemiaData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "ChronicAnemia": {
      "ReductionFactor": {
        "Scalar0To1": {
          "Value": 0.3
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### COPD
@copybrief ChronicObstructivePulmonaryDiseaseData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "ChronicObstructivePulmonaryDisease": {
      "BronchitisSeverity": {
        "Scalar0To1": {
          "Value": 0.1
        }
      },
      "EmphysemaSeverity": {
        "Scalar0To1": {
          "Value": 0.1
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Chronic Pericardial Effusion
@copybrief ChronicPericardialEffusionData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "ChronicPericardialEffusion": {
      "AccumulatedVolume": {
        "ScalarVolume": {
          "Value": 50.0,
          "Unit": "mL"
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Chronic Renal Stenosis
@copybrief ChronicRenalStenosisData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "ChronicRenalStenosis": {
      "LeftKidneySeverity": {
        "Scalar0To1": {
          "Value": 0.1
        }
      },
      "RightKidneySeverity": {
        "Scalar0To1": {
          "Value": 0.05
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Chronic Ventricular Systolic Dysfunction
@copybrief ChronicVentricularSystolicDysfunctionData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "ChronicVentricularSystolicDysfunction": { }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Consume Meal
@copybrief ConsumeMealData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "ConsumeMeal": {
      "Meal": {
        "Nutrition": {
          "Carbohydrate": { ScalarMass": { "Value": 390.0 "Unit": "g"} },
          "CarbohydrateDigestionRate": { ScalarMassPerTime": { "Value": 0.5 "Unit": "g/min"} },
          "Fat": { ScalarMass": { "Value": 90.0 "Unit": "g"} },
          "FatDigestionRate": { ScalarMassPerTime": { "Value": 0.055 "Unit": "g/min"} },
          "Protein": { ScalarMass": { "Value": 56.0 "Unit": "g"} },
          "ProteinDigestionRate": { ScalarMassPerTime": { "Value": 0.071 "Unit": "g/min"} },
          "Calcium": { ScalarMass": { "Value": 1.0 "Unit": "g"} },
          "Sodium": { ScalarMass": { "Value": 1.5 "Unit": "g"} },
          "Water": { ScalarVolume": { "Value": 3.7 "Unit": "L"} }
        }
        "ElapsedTime": { "ScalarTime": { "Value": 2.0 "Unit": "hr"} }
      }
    }
  }
}]

#or

# file must be in the ./bin/nutrition directory
"AnyCondition": [{
  "PatientCondition": {
    "ConsumeMeal": {
      "Meal": {
        "NutritionFile": "./nutrition/Soylent.json"
      }
    }
  }
}]

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Impaired Alveolar Exchange
Specify one of 3 different parameters
@copybrief ImpairedAlveolarExchangeData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "ImpairedAlveolarExchange": {
      "ImpairedSurfaceArea":  "ScalarArea": { "Value": 55.9896, "Unit": "m^2" } 
      }
    }
  }
}]

or

"AnyCondition": [{
  "PatientCondition": {
    "ImpairedAlveolarExchange": {
      "ImpairedFraction": { "Scalar0To1": { "Value": 0.8 } }
    }
  }
}]

or

"AnyCondition": [{
  "PatientCondition": {
    "ImpairedAlveolarExchange": {
      "Severity": { "Scalar0To1": { "Value": 0.2 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Pneumonia
@copybrief PneumoniaData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "Pneumonia": {
      "Severity": [ 
      { "Compartment": "LeftLung", "Severity": { "Scalar0To1": { "Value": 0.8 }}},
      { "Compartment": "RightLung", "Severity": { "Scalar0To1": { "Value": 0.8 }}}]
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Pulmonary Shunt
@copybrief PulmonaryShuntData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "PulmonaryShunt": {
      "Severity": {
        "Scalar0To1": {
          "Value": 0.8
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Pulmonary Fibrosis
@copybrief PulmonaryFibrosisData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "PulmonaryFibrosis": {
      "Severity": {
        "Scalar0To1": {
          "Value": 0.8
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Sepsis
@copybrief SepsisData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "PatientCondition": {
    "Sepsis": { 
      "Severity": { "Scalar0To1": { "Value": 0.6 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Initial Environment
@copybrief InitialEnvironmentData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyCondition": [{
  "EnvironmentCondition": {
    "InitialEnvironmentConditions": {
      "Conditions": {
        "SurroundingType": "Air",
        "AirVelocity": { "ScalarLengthPerTime": { "Unit": "m/s" } },
        "AmbientTemperature": { "ScalarTemperature": { "Value": 22.0, "Unit": "degC" } },
        "AtmosphericPressure": { "ScalarPressure": { "Value": 525.0, "Unit": "mmHg" } },
        "ClothingResistance": { "ScalarHeatResistanceArea": { "Value": 0.5, "Unit": "clo" } },
        "Emissivity": { "Scalar0To1": { "Value": 0.9 } },
        "MeanRadiantTemperature": { "ScalarTemperature": { "Value": 22.0, "Unit": "degC" } },
        "RelativeHumidity": { "Scalar0To1": { "Value": 0.5 } },
        "RespirationAmbientTemperature": { "ScalarTemperature": { "Value": 22.0, "Unit": "degC" } },
        "AmbientGas": [
          { "Name": "Nitrogen", "Amount": { "Scalar0To1": { "Value": 0.7896 } } }, 
          { "Name": "Oxygen", "Amount": { "Scalar0To1": { "Value": 0.21 } } },
          { "Name": "CarbonDioxide", "Amount": { "Scalar0To1": { "Value": 4.0E-4 } } }
        ]
      }
    }
  }
}]

#or

#File must be in the ./bin/environments directory 
"AnyCondition": [{
  "EnvironmentCondition": {
    "InitialEnvironmentConditions": {
      "ConditionsFile": "./environments/Hypobaric3000m.json"
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - - 

### Data Requests

Currently there are four supported types of data requests:

#### Physiology System Data

Physiology System data refers to all the data specified by SystemData and its derived types.

At this time, you do not need to specify the system name.
The Name attribute should be set to a System Property name. (e.g., HeartRate)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{"Category":"Physiology", "PropertyName":"HeartRate", "Unit":"1/min", "DecimalFormat":{"Precision":2} },
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Compartment Data

Compartments refer to all the data specified on Compartments.
You can read more about compartments @ref CompartmentsInterface "here".

There are two main types of Compartments, gas and liquid.

Data on the compartment itself: 
The Compartment attribute can be any of the enumerations defined. 
The Name attribute should be set to a Compartment Property name.
The Substance attribute is optional, and the if used the name will refer to a substance quantity property.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category":"GasCompartment", "CompartmentName"":"Lungs", "PropertyName":"Pressure", "Unit":"cmH2O", "DecimalFormat":{"Precision":2} },
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category":"GasCompartment", "CompartmentName":"Lungs", "SubstanceName":"Oxygen", "PropertyName":"PartialPressure", "Unit":"mmHg", "DecimalFormat":{"Precision":2} },
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category":"LiquidCompartment", "CompartmentName":"Aorta", "PropertyName":"Pressure", "Unit":"mmHg", "DecimalFormat":{"Precision":2} },
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category":"LiquidCompartment", "CompartmentName":"Aorta", "SubstanceName":"Oxygen", "PropertyName":"PartialPressure", "Unit":"mmHg", "DecimalFormat":{"Precision":2} },
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Environment Data

%Environment System data refers to all the data specified by @ref Environment_EnvironmentData and its derived types.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category":"Environment", "PropertyName":"AirDensity", "Unit":"g/mL" }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Equipment Data

System level data from a piece of equipment

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category":"AnesthesiaMachine", "PropertyName":"InletFlow", "Unit":"mL/min" },

{ "Category":"BagValveMask", "PropertyName":"FilterVolume", "Unit":"mL" },

{ "Category":"ECG", PropertyName:"Lead3ElectricPotential", "Unit":"1/min" },

{ "Category":"Inhaler", "PropertyName":"MeteredDose", "Unit":"ug" },
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Substance Data

Substance data is provided about a substance and its current state in the body and on specific anatomy compartments

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category":"Substance", "SubstanceName":"Oxygen", "PropertyName":"AlveolarTransfer", "Unit":"mL/s" },
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category":"Substance", "SubstanceName":"CarbonDioxide", "PropertyName":"AlveolarTransfer", "Unit":"mL/s" },
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Action Data

Actions can contain parameters that update as the actions persists in the engine.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category": "Action", "ActionName": "Hemorrhage", "CompartmentName": "RightLegVasculature", "PropertyName": "FlowRate", "Unit": "mL/s"},
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{ "Category": "Action", "ActionName": "Hemorrhage", "CompartmentName": "RightLegVasculature", "PropertyName": "TotalBloodLost", "Unit": "mL"},
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

### General Actions

Actions give instructions to the engine to define what happens over the course of a scenario. 
Everything from advancing time, to starting a hemorrhage, to administering a drug is an action of some kind. 
The following are links to the Action class specification along with XML examples of actions that can be used in making your own scenarios.

- - -

##### Advance Time

@copybrief AdvanceTimeData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AdvanceTime": {
    "Time": { "ScalarTime": { "Value": 5.0, "Unit": "min" } }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Serialize State
@copybrief SerializeStateData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "Serialize": {
    "Type": "Save",
    "Filename": "State.json" # If no filename is provided, engine is going to auto generate a name, something like : StandardMale@0s.xml
  }
}]

"AnyAction": [{
  "Serialize": {
    "Type": "Load",
    "Filename": "State.json"
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##### Override

@copybrief OverridesData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "Overrides": {
    "ScalarOverride": [{ "Name":"", "Value": 5.0, "Unit": "min" },
                       { "Name":"", "Value": 5.0, "Unit": "min" }]
    }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

### Patient Actions
- - -

##### ARDS Exacerbation
@copybrief AcuteRespiratoryDistressSyndromeExacerbationData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "AcuteRespiratoryDistressSyndromeExacerbation": {
      "Severity": [ 
        { "Compartment": "LeftLung", "Severity": { "Scalar0To1": { "Value": 0.3 }}},
        { "Compartment": "RightLung", "Severity": { "Scalar0To1": { "Value": 0.6 }}}]
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Acute Stress
@copybrief AcuteStressData <br>
Severity value must be >=0.0 and <=1.0 <br>
A severity of 0 removes the action completely.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "AcuteStress": {
      "Severity": { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Airway Obstruction 
@copybrief AirwayObstructionData <br>
Severity value must be >=0.0 and <=1.0 <br>
A severity of 0 removes the action completely.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "AirwayObstruction": {
      "Severity { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Arrhythmia
@copybrief ArrhythmiaData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "Arrhythmia": { "Rhythm":"Asystole" }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Asthma Attack
@copybrief AsthmaAttackData <br>
Severity value must be >=0.0 and <=1.0 <br>
A severity of 0 removes the action completely.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "AsthmaAttack": {
      "Severity": { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Brain Injury
@copybrief BrainInjuryData <br>
Severity value must be >=0.0 and <=1.0 <br>
A severity of 0 removes the action completely.<br>
Types : Diffuse, LeftFocal, RightFocal
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "BrainInjury": {
      "Type":"LeftFocal",
      "Severity": { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -
  
##### Bronchoconstriction 
@copybrief BronchoconstrictionData <br>
Severity value must be >=0.0 and <=1.0 <br>
A severity of 0 removes the action completely.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "Bronchoconstriction": {
      "Severity": { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Chest Compression 
@copybrief ChestCompressionData <br>
Must provide one of Force or ForceScale<br>
Note, that patient should be in Cardiac Arrest before performing CPR
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "ChestCompressionInstantaneous": {
     "PatientAction": {
      "Action": {}
     },
     "Force": {
      "ScalarForce": {
       "Value": 233.55,
       "Unit": "N"
      }
     },
     "CompressionPeriod": {
      "ScalarTime": {
       "Value": 0.4,
       "Unit": "s"
      }
     }
    }
  }
}]

or

"AnyAction": [{
  "PatientAction": {
    "ChestCompressionInstantaneous": {
     "PatientAction": {
      "Action": {}
     },
     "ForceScale": {
      "Scalar0To1": {
       "Value": 0.31
      }
     },
     "CompressionPeriod": {
      "ScalarTime": {
       "Value": 0.4,
       "Unit": "s"
      }
     }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -
##### Chest Compression Automated 
@copybrief ChestCompressionAutomatedData <br>
Must provide one of Force or ForceScale<br>
AppliedForceFraction is OPTIONAL<br>
Note, that patient should be in Cardiac Arrest before performing CPR
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "ChestCompressionAutomated": {
     "PatientAction": {
      "Action": {}
     },
     "ForceScale": {
      "Scalar0To1": {
       "Value": 0.6228
      }
     },
     "CompressionFrequency": {
      "ScalarFrequency": {
       "Value": 120,
       "Unit": "1/min"
      }
     },
     "AppliedForceFraction": {
      "Scalar0To1": {
       "Value": 0.8
      }
     }
    }
  }
}]

or

"AnyAction": [{
  "PatientAction": {
    "ChestCompressionAutomated": {
     "PatientAction": {
      "Action": {}
     },
     "Force": {
      "ScalarForce": {
       "Value": 233.55,
       "Unit": "N"
      }
     },
     "CompressionFrequency": {
      "ScalarFrequency": {
       "Value": 120,
       "Unit": "1/min"
      }
     },
     "AppliedForceFraction": {
      "Scalar0To1": {
       "Value": 0.8
      }
     }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -
  
##### Chest Compression Instantaneous 
@copybrief ChestCompressionInstantaneousData <br>
Must provide one of Force or ForceScale<br>
Note, that patient should be in Cardiac Arrest before performing CPR
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "ChestCompressionInstantaneous": {
     "PatientAction": {
      "Action": {}
     },
     "Force": {
      "ScalarForce": {
       "Value": 233.55,
       "Unit": "N"
      }
     }
    }
  }
}]

or

"AnyAction": [{
  "PatientAction": {
    "ChestCompressionInstantaneous": {
     "PatientAction": {
      "Action": {}
     },
     "ForceScale": {
      "Scalar0To1": {
       "Value": 0.31
      }
     }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Chest Occlusive Dressing 
@copybrief ChestOcclusiveDressingData <br>
The State attribute can be "On" or "Off" <br>
Side is either Left or Right<br>
FATAL: If the side specified does not have a pneumothorax 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "ChestOcclusiveDressing": {
      "State":"On",
      "Side":"Right"
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Conscious Respiration 
@copybrief ConsciousRespirationData <br>
This action can contain 1 or more commands :
- @copybrief ForcedInhaleData
- @copybrief ForcedExhaleData
- @copybrief ForcedPauseData
- @copybrief UseInhalerData

<br>
Commands will be processed in order.
The first commands will be processed instantly
When it has completed (run through it's Period),
the next command will be processed.
Other actions will be processed while these commands
are processed or waiting to be processed.
You may want to advance time for the sum of the 
command periods to ensure the body is doing what you 
expect it to.. Or not, depending on how you want 
the system to react.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "ConsciousRespiration": {
      "Command": {
        "ForcedExhale": {
          "ExpiratoryReserveVolumeFraction": { "Scalar0To1": { "Value":1.0 } },
          "Period": { "ScalarTime": { "Value":3.0, "Unit":"s"} }
        }
      }
      "Command": {
        "UseInhaler": {}
      }
      "Command": {
        "ForcedInhale": {
          "InspiratoryCapacityFraction": { "Scalar0To1": { "Value":1.0 } },
          "Period": { "ScalarTime": { "Value":5.0, "Unit":"s" } }
        }
      }
      "Command": {
        "ForcedPause": {
          "Period": { "ScalarTime": { "Value":10.0, "Unit":"s" } }
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### COPD Exacerbation
@copybrief ChronicObstructivePulmonaryDiseaseExacerbationExacerbationData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "ChronicObstructivePulmonaryDiseaseExacerbation": {
      "BronchitisSeverity": { "Scalar0To1": { "Value": 0.5 }},
      "EmphysemaSeverity": { "Scalar0To1": { "Value": 0.7 }}
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


- - -

##### Consume Nutrients 
@copybrief ConsumeNutrientsData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "ConsumeNutrients": {
      "Nutrition": {
        "Carbohydrate": { "ScalarMass": { "Value":390.0, "Unit":"g" } },
        "CarbohydrateDigestionRate": { "ScalarMassPerTime": { "Value":0.5, "Unit":"g/min" } },
        "Fat": { "ScalarMass": { "Value":90.0, "Unit":"g" } },
        "FatDigestionRate": { "ScalarMassPerTime": { "Value":0.055, "Unit":"g/min" } },
        "Protein": { "ScalarMass": { "Value":56.0, "Unit":"g"} },
        "ProteinDigestionRate": { "ScalarMassPerTime": { "Value":0.071, "Unit":"g/min" } },
        "Calcium": { "ScalarMass": { "Value":1.0, "Unit":"g"} },
        "Sodium": { "ScalarMass": { "Value":1.5, "Unit":"g"} },
        "Water": { "ScalarVolume": { "Value":3.7, "Unit":"L"} }
      }
    }
  }
}]

#or

"AnyAction": [{
  "PatientAction": {
    "ConsumeNutrients": {
      "NutritionFile": { "./nutrition/Soylent.json" }
      }
    }
  }]

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- - -

##### Dyspnea
@copybrief DyspneaData <br>
Severity value must be >=0.0 and <=1.0 <br>
A severity of 0 removes the action completely.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "Dyspnea": {
      "Severity": { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Exercise 
@copybrief ExerciseData <br>
An intensity of 0 removes the action completely.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "Exercise": {
      "Intensity": { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Hemorrhage 
@copybrief HemorrhageData <br>
The Compartment attribute can be any of the enumerations defined in the enumAnatomy enumeration.<br>
Type can be Internal or External
FATAL: Cannot have bleeding rate greater than cardiac output or less than 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "Hemorrhage": { 
      "Compartment":"RightLeg",
      "Type":"External",
      "Severity": { "Scalar0To1": { "Value":0.25 } }
    }
  }
}]

or

"AnyAction": [{
  "PatientAction": {
    "Hemorrhage": { 
      "Compartment":"RightLeg",
      "Type":"External",
      "Rate": { "ScalarVolumePerTime": { "Value":250.0, "Unit":"mL/min" } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Impaired Alveolar Exchange Exacerbation 
@copybrief ImpairedAlveolarExchangeExacerbationData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "ImpairedAlveolarExchangeExacerbation": {
      "ImpairedSurfaceArea": {
        "ScalarArea": {
          "Value": 25,  "Unit": "m^2"
        }
      }
    }
  }
}]

or

"AnyAction": [{
  "PatientAction": {
    "ImpairedAlveolarExchangeExacerbation": {
      "ImpairedFraction": {
        "Scalar0To1": {
          "Value": 0.09
        }
      }
    }
  }
}]


or

"AnyAction": [{
  "PatientAction": {
    "ImpairedAlveolarExchangeExacerbation": {
      "Severity": {
        "Scalar0To1": {
          "Value": 0.5
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Intubation 
@copybrief IntubationData <br>
Note: In order to 'turn off' an intubation, use'Off' as the Type  <br>
Types : Off, Esophageal, LeftMainstem, RightMainstem, Tracheal, Oropharyngeal, Nasopharyngeal
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "Intubation": { "Type":"Tracheal" }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Pneumonia Exacerbation
@copybrief PneumoniaExacerbationData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "PneumoniaExacerbation": {
      "Severity": [ 
        { "Compartment": "LeftLung", "Severity": { "Scalar0To1": { "Value": 0.8 }}},
        { "Compartment": "RightLung", "Severity": { "Scalar0To1": { "Value": 0.8 }}}]
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Mechanical Ventilation 
@copybrief MechanicalVentilationData <br>
You may provide Pressure and/or Flow. <br>
If you do not provide GasFractions, the environment gas fractions will be used. <br>
If you do provide Gas Fractions, they must add up to 1.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "MechanicalVentilation": { "State":"On",
      "Flow": { "ScalarVolumePerTime": { "Value":1.0, "Unit":"mL/s" } },
      "Pressure": { "ScalarPressure": { "Value":10.0, "Unit":"cmH2O" } },
      "GasFraction": { "Name":"Oxygen",
        "Amount": { "Scalar0To1": { "Value":0.3 } } },
      "GasFraction": { "Name":"CarbonDioxide",
        "Amount": { "Scalar0To1": { "Value":0.1 } } },
      "GasFraction": { "Name":"Nitrogen",
        "Amount": { "Scalar0To1": { "Value":0.6 } } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Needle Decompression 
@copybrief NeedleDecompressionData <br>
The Side attribute can be "Left" or "Right"<br>
The State attribute can be "On" or "Off"
FATAL: If the side specified does not have a pneumothorax 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "NeedleDecompression": {
      "State":"On",
      "Side":"Right"
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Pericardial Effusion
@copybrief PericardialEffusionData <br>
EffusionRate of the liquid
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "PericardialEffusion": {
      "EffusionRate": { "ScalarVolumePerTime": { "Value":0.1, "Unit":"mL/s" } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Pulmonary Shunt Exacerbation
@copybrief PulmonaryShuntExacerbationData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "PulmonaryShuntExacerbation": {
      "Severity": {
        "Scalar0To1": {
          "Value": 0.8
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Respiratory Mechanics Configuration
@copybrief RespiratoryMechanicsConfigurationData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "RespiratoryMechanicsConfiguration": {
     "PatientAction": {
      "Action": {}
     },
     "Settings": {
      "Active": "On",
      "LeftComplianceCurve": {
       "Segment": [
        {
         "ConstantSegment": {
          "BeginVolume": { "ScalarVolume": { "Value": "-Infinity", "Unit": "mL" } },
          "EndVolume": { "ScalarVolume": { "Value": "Infinity", "Unit": "mL" } },
          "Compliance": { "ScalarVolumePerPressure": { "Value": 25, "Unit": "mL/cmH2O" } }
         }}]
      },
      "RightComplianceCurve": {
       "Segment": [
        {
         "ConstantSegment": { "BeginVolume": { "ScalarVolume": { "Value": "-Infinity", "Unit": "mL" } },
          "EndVolume": { "ScalarVolume": { "Value": "Infinity", "Unit": "mL" } },
          "Compliance": { "ScalarVolumePerPressure": { "Value": 25, "Unit": "mL/cmH2O" } }
         }}]
      },
      "LeftExpiratoryResistance": { "ScalarPressureTimePerVolume": { "Value": 6.5, "Unit": "cmH2O s/L" } },
      "LeftInspiratoryResistance": { "ScalarPressureTimePerVolume": { "Value": 6.5, "Unit": "cmH2O s/L" } },
      "RightExpiratoryResistance": { "ScalarPressureTimePerVolume": { "Value": 6.5, "Unit": "cmH2O s/L" } },
      "RightInspiratoryResistance": { "ScalarPressureTimePerVolume": { "Value": 6.5, "Unit": "cmH2O s/L" } },
      "UpperExpiratoryResistance": { "ScalarPressureTimePerVolume": { "Value": 9.75, "Unit": "cmH2O s/L" } },
      "UpperInspiratoryResistance": { "ScalarPressureTimePerVolume": { "Value": 9.75, "Unit": "cmH2O s/L" } },
      "InspiratoryPeakPressure": { "ScalarPressure": { "Value": -13, "Unit": "cmH2O" } },
      "ExpiratoryPeakPressure": { "ScalarPressure": { "Unit": "cmH2O" } },
      "InspiratoryRiseTime": { "ScalarTime": { "Value": 0.9, "Unit": "s" } },
      "InspiratoryHoldTime": { "ScalarTime": { "Unit": "s" } },
      "InspiratoryReleaseTime": { "ScalarTime": { "Value": 0.4, "Unit": "s" } },
      "InspiratoryToExpiratoryPauseTime": { "ScalarTime": { "Value": 2.7, "Unit": "s" } },
      "ExpiratoryRiseTime": { "ScalarTime": { "Unit": "s" } },
      "ExpiratoryHoldTime": { "ScalarTime": { "Unit": "s" } },
      "ExpiratoryReleaseTime": { "ScalarTime": { "Unit": "s" } }
     }}}
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Respiratory Fatigue
@copybrief RespiratoryFatigueData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "RespiratoryFatigue": {
      "Severity": { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Substance Bolus  
@copybrief SubstanceBolusData

The AdminRoute can be one of:
- "Intravenous"
- "Intramuscular"
- "Subcutaneous"
- "Oral"
- "Rectal"
- "Inhaled"

The Substance element should be set to a name of any of the %Substances.
AdministrationDuration is optional, a default will be provided by the model.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "SubstanceBolus": {
      "AdministrationRoute":"Intravenous",
      "Substance":"Succinylcholine",
      "Concentration": { "ScalarMassPerVolume": { "Value":4820.0, "Unit":"ug/mL" } },
      "Dose": { "ScalarVolume": { "Value":30.0, "Unit":"mL" } },
      "AdministrationDuration": { "ScalarTime": { "Value":2.0, "Unit":"s" } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Substance Compound Infusion Fluids 
@copybrief SubstanceCompoundInfusionData <br>
The Substance Compound element should be set to a name of any of the %Substances Compounds. <br>
Set Rate to 0 to remove Action
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "SubstanceCompoundInfusion": {
      "SubstanceCompound":"Saline",
      "BagVolume": { "ScalarVolume": { "Value":500.0, "Unit":"mL" } },
      "Rate": { "ScalarVolumePerTime": { "Value":100.0, "Unit":"mL/min" } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Substance Infusion 
@copybrief SubstanceInfusionData <br>
The Substance element should be set to a name of any of the %Substances. <br>
Set Rate to 0 to remove Action
Volume is optional, if not provided a default will be given by the model.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "SubstanceInfusion": { 
     "Substance":"Succinylcholine",
      "Concentration": { "ScalarMassPerVolume": { "Value":5000.0, "Unit":"ug/mL"} },
      "Rate": { "ScalarVolumePerTime": { "Value":100.0, "Unit":"mL/min" } },
      "Volume": { "ScalarVolume": { "Value":100.0, "Unit":"mL" } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Supplemental Oxygen 
@copybrief SupplementalOxygenData <br>
You must provide a Device <br>
Default Flow and Volume will be used depending on device type if not provided<br>
A Volume of "Infinity" may be used for something like a wall port
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
      "SupplementalOxygenData": {
        "Device": "SimpleMask"
      }
    }
}]

"AnyAction": [{
  "PatientAction": {
      "SupplementalOxygenData": {
        "Device": "SimpleMask",
        "Flow": { "ScalarVolumePerTime": { "Value": 5.0, "Unit": "L/s" }},
        "Volume": { "ScalarVolume": { "Value": "Infinity", "Unit": "L" }}
      }
    }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Tension Pneumothorax 
@copybrief TensionPneumothoraxData <br>
The Type attribute can be "Open" or "Closed"<br>
The Side attribute can be "Left" or "Right"<br>
Severity value must be >=0.0 and <=1.0 <br>
A severity of 0 removes the action completely.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "TensionPneumothorax": {
      "Type":"Open,
      "Side":"Right,
      "Severity": { "Scalar0To1": { "Value":0.3 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Urinate 
@copybrief UrinateData <br>
Action to empty the bladder. if not emptied, 
it will empty and throw an event.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "PatientAction": {
    "Urinate": {}
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

### Anesthesia Machine State

- - -
##### Anesthesia Machine Configuration 
@copybrief AnesthesiaMachineConfigurationData <br>
NOTE: Each field is optional. <br>
Connection can be one of : Off, On <br>
Cannot have any other equipment on at the same time <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{ {
  "AnesthesiaMachineAction": {
    "Configuration": {
      "Configuration": {
        "Connection":"On",
        "InletFlow": { "ScalarVolumePerTime": { "Value":5.0, "Unit":"L/min" } },
        "InspiratoryExpiratoryRatio": { "Value":0.5 },
        "OxygenFraction": { "Scalar0To1": { "Value":0.23} },
        "OxygenSource":"Wall",
        "PositiveEndExpiratoryPressure": { "ScalarPressure": { "Value":1.0, "Unit":"cmH2O"} },
        "PrimaryGas":"Nitrogen",
        "RespiratoryRate": { "ScalarFrequency": { "Value":16.0, "Unit":"1/min" } },
        "VentilatorPressure": { "ScalarPressure": { "Value":10.5, "Unit":"cmH2O" } },
        "LeftChamber": {
          "State":"On",
          "SubstanceFraction": { "Scalar0To1": { "Value":0.04 } },
          "Substance":"Desflurane"
        }
        "RightChamber": {
          "State":"On",
          "SubstanceFraction": { "Scalar0To1": { "Value":0.04 } },
          "Substance":"Desflurane"
        }
        "OxygenBottleOne": {
          "Volume": { "ScalarVolume": { "Value":660.0, "Unit":"L"} }
        }
        "OxygenBottleTwo": {
          "Volume": { "ScalarVolume" { "Value":660.0, "Unit":"L" } }
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "Configuration": {
      "Configuration": {
        "Connection":"Off"
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "Configuration": {
      "Configuration": {
        "RightChamber": {
          "State": "On",
          "SubstanceFraction": { "Scalar0To1": { "Value":0.04 } },
          "Substance":"Desflurane"
        }
        "LeftChamber": {
          "State":"Off",
          "SubstanceFraction": { "Scalar0To1": { "Value":0.01 } },
          "Substance": "Desflurane"
        }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
- - -

### Anesthesia Machine Incidents

- - -

##### Oxygen TankPressure Loss 
@copybrief OxygenTankPressureLossData <br>
The State attribute can be "On" or "Off"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "OxygenTankPressureLoss": { "State":"On" }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Oxygen Wall Port Pressure Loss 
@copybrief OxygenWallPortPressureLossData <br>
The State attribute can be "On" or "Off"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "OxygenWallPortPressureLoss": { "State":"On" }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

### Anesthesia Machine Failures

- - -

##### Expiratory Valve Leak 
@copybrief ExpiratoryValveLeakData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "ExpiratoryValveLeak": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Expiratory Valve Obstruction 
@copybrief ExpiratoryValveObstructionData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "ExpiratoryValveObstruction": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Inspiratory Valve Leak 
@copybrief InspiratoryValveLeakData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "InspiratoryValveLeak": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Inspiratory Valve Obstruction 
@copybrief InspiratoryValveObstructionData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "InspiratoryValveObstruction": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Mask Leak 
@copybrief MaskLeakData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "MaskLeak": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Soda Lime Failure 
@copybrief SodaLimeFailureData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "SodaLimeFailure": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Tube Cuff Leak 
@copybrief TubeCuffLeakData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "TubeCuffLeak": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Vaporizer Failure 
@copybrief VaporizerFailureData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "VaporizerFailure": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Ventilator Pressure Loss 
@copybrief VentilatorPressureLossData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "VentilatorPressureLoss": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Y Piece Disconnect 
@copybrief YPieceDisconnectData <br>
The State attribute can be "On" or "Off"<br>
Severity value must be >=0.0 and <=1.0 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "AnesthesiaMachineAction": {
    "YPieceDisconnect": {
      "Severity": { "Scalar0To1": { "Value":0.5 } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

### BagValveMask

##### BagValveMask Configuration 
@copybrief BagValveMaskConfigurationData <br>
FATAL: Cannot have bvm, mechanical ventilator, and anesthesia machine on at the same time
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EquipmentAction": {
    "BagValveMaskConfiguration": {
      "BagValveMaskAction": { "EquipmentAction": { "Action": {
        "Comment": "Attach the bag valve mask" }}},
        "Configuration":
        {
          "Connection": "On",
          "ValvePositiveEndExpiratoryPressure": { "ScalarPressure": { "Value": 5.0, "Unit": "cmH2O" } }
        }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### BagValveMask Automated 
@copybrief BagValveMaskAutomatedData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": 
  "EquipmentAction": {
    "BagValveMaskAutomated": {
      "BagValveMaskAction": { "EquipmentAction": { "Action": {
        "Comment": "Automate a squeeze pressure to give TV ~ 7 mL/kg(ideal)" }}},
        "BreathFrequency": { "ScalarFrequency": { "Value": 12, "Unit": "1/min" } },
        "InspiratoryExpiratoryRatio": { "Value": 0.5, "Unit": "" },
        "SqueezePressure": { "ScalarPressure": { "Value": 12.3, "Unit": "cmH2O" } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### BagValveMask Instantaneous 
@copybrief BagValveMaskInstantaneousData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{ 
  "EquipmentAction": {
    "BagValveMaskInstantaneous": {
      "BagValveMaskAction": { "EquipmentAction": { "Action": {} } },
      "Pressure": { "ScalarPressure": { "Value": 5.0, "Unit": "cmH2O" } } } }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### BagValveMask Squeeze 
@copybrief BagValveMaskSqueezeData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EquipmentAction": {
    "BagValveMaskSqueeze": {
      "BagValveMaskAction": { "EquipmentAction": { "Action": {} } },
        "SqueezePressure": { "ScalarPressure": { "Value": 15.0, "Unit": "cmH2O" } },
        "ExpiratoryPeriod": { "ScalarTime": { "Value": 4.0, "Unit": "s" } }, 
        "InspiratoryPeriod": { "ScalarTime": { "Value": 6.0, "Unit": "s" } }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

### Inhaler

##### Inhaler Configuration 
@copybrief InhalerConfigurationData <br>
FATAL: Cannot have bvm, mechanical ventilator, and anesthesia machine on with inhaler
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "InhalerAction": {
    "Configuration": {
      "Configuration": {
        "Substance": "Albuterol",
        "MeteredDose": { "ScalarMass": { "Value":90.0, "Unit":"ug" } }
        "NozzleLoss": { "Scalar0To1": { "Value":0.04 } }
        "SpacerVolume": { "ScalarVolume": { "Value":500.0 "Unit":"mL" } }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Mechanical Ventilator

##### Mechanical Ventilator Configuration 
@copybrief MechanicalVentilatorConfigurationData <br>
FATAL: Cannot have bvm, inhaler, and anesthesia machine on at the same time
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EquipmentAction": {
    "MechanicalVentilatorConfiguration": {
      "MechanicalVentilatorAction": { "EquipmentAction": { "Action": {
            "Comment": "Attach the mechanical ventilator with rate = 20 bpm and I:E Ratio = 1:1" }}},
      "Settings":
      {
        "Connection": "On",
        "InspirationWaveform": "Square",
        "ExpirationWaveform": "Square",
        "PeakInspiratoryPressure": { "ScalarPressure": { "Value": 20.0, "Unit": "cmH2O" } },
        "PositiveEndExpiratoryPressure": { "ScalarPressure": { "Value": 10.0, "Unit": "cmH2O" } },
        "InspirationTriggerTime": { "ScalarTime": { "Value": 1.5, "Unit": "s" } },
        "ExpirationCycleTime": { "ScalarTime": { "Value": 1.5, "Unit": "s" } },
        "FractionInspiredGas": [
          { "Name": "Oxygen", "Amount": { "Scalar0To1": { "Value": 0.3, "Unit": "" } } } ],
        "ConcentrationInspiredAerosol": [
          { "Name": "Albuterol", "Concentration": { "ScalarMassPerVolume": { "Value": 2500.0, "Unit": "mg/m^3" } } }]
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Mechanical Ventilator Hold 
@copybrief MechanicalVentilatorHoldData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EquipmentAction": {
    "MechanicalVentilatorHold": {
     "MechanicalVentilatorAction": {
      "EquipmentAction": { "Action": {} } },
     "State": "On",
     "AppliedRespiratoryCycle": "Inspiration"
    }
   }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Mechanical Ventilator Leak 
@copybrief MechanicalVentilatorLeakData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EquipmentAction": {
    "MechanicalVentilatorLeak": {
     "MechanicalVentilatorAction": {
      "EquipmentAction": { "Action": {} } },
     "Severity": { "Scalar0To1": { "Value": 0.42 } }
    }
   }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Mechanical Ventilator CPAP 
@copybrief MechanicalVentilatorContinuousPositiveAirwayPressureData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EquipmentAction": {
    "MechanicalVentilatorContinuousPositiveAirwayPressure": {
     "MechanicalVentilatorMode": {
      "MechanicalVentilatorAction": {
       "EquipmentAction": { "Action": {} } },
      "Connection": "On" },
     "DeltaPressureSupport": { "ScalarPressure": { "Value": 10, "Unit": "cmH2O" } },
     "FractionInspiredOxygen": { "Scalar0To1": { "Value": 0.21 } },
     "PositiveEndExpiratoryPressure": { "ScalarPressure": { "Value": 5, "Unit": "cmH2O" } },
     "Slope": { "ScalarTime": { "Value": 0.2, "Unit": "s" } }
    }
   }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Mechanical Ventilator Pressure Control 
@copybrief MechanicalVentilatorPressureControlData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EquipmentAction": {
    "MechanicalVentilatorPressureControl": {
     "MechanicalVentilatorMode": {
      "MechanicalVentilatorAction": {
       "EquipmentAction": { "Action": {} } },
      "Connection": "On" },
     "FractionInspiredOxygen": { "Scalar0To1": { "Value": 0.21 } },
     "InspiratoryPeriod": { "ScalarTime": { "Value": 1, "Unit": "s" } },
     "InspiratoryPressure": { "ScalarPressure": { "Value": 19, "Unit": "cmH2O" } },
     "PositiveEndExpiratoryPressure": { "ScalarPressure": { "Value": 5, "Unit": "cmH2O" } },
     "RespirationRate": { "ScalarFrequency": { "Value": 12, "Unit": "1/min" } },
     "Slope": { "ScalarTime": { "Unit": "s" } }
     }
   }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Mechanical Ventilator Volume Control
@copybrief MechanicalVentilatorVolumeControlData <br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EquipmentAction": {
    "MechanicalVentilatorVolumeControl": {
     "MechanicalVentilatorMode": {
      "MechanicalVentilatorAction": {
       "EquipmentAction": { "Action": {} } },
      "Connection": "On" },
     "Flow": { "ScalarVolumePerTime": { "Value": 60, "Unit": "L/min" } },
     "FractionInspiredOxygen": { "Scalar0To1": { "Value": 0.21 } },
     "InspiratoryPeriod": { "ScalarTime": { "Value": 1, "Unit": "s" } },
     "PositiveEndExpiratoryPressure": { "ScalarPressure": { "Value": 5, "Unit": "cmH2O" } },
     "RespirationRate": { "ScalarFrequency": { "Value": 12, "Unit": "1/min" } },
     "TidalVolume": { "ScalarVolume": { "Value": 900, "Unit": "mL" } }
    }
   }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

### Environment 

- - -

##### Environment Configuration State
@copybrief EnvironmentChangeData <br>
NOTE: Each field is optional.
You can also add particulates to the surrounding atmosphere
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EnvironmentAction" {
    "Conditions": {
      "Conditions": {
        "SurroundingType": "Air",
        "AirVelocity": { "ScalarLengthPerTime": { "Unit": "m/s" } },
        "AmbientTemperature": { "ScalarTemperature": { "Value": 22.0, "Unit": "degC" } },
        "AtmosphericPressure": { "ScalarPressure": { "Value": 525.0, "Unit": "mmHg" } },
        "ClothingResistance": { "ScalarHeatResistanceArea": { "Value": 0.5, "Unit": "clo" } },
        "Emissivity": { "Scalar0To1": { "Value": 0.9 } },
        "MeanRadiantTemperature": { "ScalarTemperature": { "Value": 22.0, "Unit": "degC" } },
        "RelativeHumidity": { "Scalar0To1": { "Value": 0.5 } },
        "RespirationAmbientTemperature": { "ScalarTemperature": { "Value": 22.0, "Unit": "degC" } },
        "AmbientGas": [
          { "Name": "Nitrogen", "Amount": { "Scalar0To1": { "Value": 0.7896 } } }, 
          { "Name": "Oxygen", "Amount": { "Scalar0To1": { "Value": 0.21 } } },
          { "Name": "CarbonDioxide", "Amount": { "Scalar0To1": { "Value": 4.0E-4 } } }
        ]
      }
    }
  }
}]

or

<!-- file must be in the ./bin/environments directory -->
"AnyAction": [{
  "EnvironmentAction": { 
    "Conditions": { "ConditionsFile":"./environments/Hypobaric3000m.json" }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- - -

##### Thermal Application
@copybrief ThermalApplicationData <br>
You must provide at least 1 activity, but up can also 
apply up-to all 3 in one action.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"AnyAction": [{
  "EnvironmentAction": {
    "ThermalApplication": {
      "ActiveHeating": {
        "Power": { "ScalarPower": { "Value":500.0, "Unit":"BTU/hr" } },
        "SurfaceAreaFraction": { "Scalar0To1": { "Value":0.2} }
      }
      "ActiveCooling": {
        "Power": { "ScalarPower": { "Value":500.0, "Unit":"BTU/hr"} },
        "SurfaceArea": { "ScalarArea": { "Value":0.1, "Unit":"m^2"} }
      }
      "AppliedTemperature": { 
        "State":"On",
        "Temperature": { "ScalarTemperature" { "Value":30.0, "Unit":"degF"} },
        "SurfaceAreaFraction": { "Scalar0To1": { "Value":1.0} }
      }
    }
  }
}]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
