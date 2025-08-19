About Pulse {#AboutPulse}
=========================

The Pulse Physiology Engine can be used as a standalone application or integrated with simulators, sensor interfaces, and models of all fidelities.
The platform includes a common data model for standard model and data definitions, a software interface for engine control, robust physics-based circuit and transport solvers, and a verification and validation suite. 
The architecture was specifically designed to reduce model development time and increase the usability of the engine in simulations by creating a modular, extensible definition for human physiology. 
Pulse provides the following benefits to its user community:

  - <b>Sound underlying physics:</b> Clear, accurate, precise first principles conservation equations.
  - <b>Standardized data model:</b> Easily understandable, widely usable ontologies and software interfaces.
  - <b>Extensible:</b> Reusable, repeatable implementation to allow the addition of new capabilities and functionality.
  - <b>Modular:</b> Interdependent, hierarchical models for varying fidelity and complexity.
  - <b>Interoperable:</b> Seamless integration with various simulators, sensors, and software platforms, with easy cross-platform deployment on all standard operating systems (Windows, Mac, Linux, and ARM) and multiple languages (C++, C#, Java, Unity).
  - <b>Credible:</b> Computational models transparently derived from evidence-based literature, analyzed with extensive verification and validation tools, ensuring reliable performance.
  - <b>Thorough documentation:</b> In-depth, referenced descriptions of physiology methodology and software design.
  - <b>Open:</b> Public repository managed by an experienced Kitware team with a permissible Apache 2.0 license, allowing users complete control without dependency on proprietary vendors, and facilitating multicenter and multidisciplinary collaborative development.
  - <b>Economical:</b> Cost-sharing model supported by contributions from government and private entities, reducing financial barriers for users.

Pulse is comprised of numerical models representing the different body systems, feedback mechanisms and interactions between the systems, PK/PD, and medical equipment. 
The major systems are modeled using zero-dimensional lumped-parameter circuit analogs (e.g., the cardiovascular circuit) with homeostatic feedback. 
The differential equations contained in each system are calculated through transient analysis with a shared dynamic time step. 
The numerical models currently execute with a time step of 20 ms, which can be reduced, as necessary, to ensure all physiology features of interest are captured, while maintaining real-time execution for the simulation.

Pulse has been developed through a combination of funding by many different government and private entities, and is a significantly improved and extended fork from the DoD-funded BioGears program.
The Pulse repository is maintained by the Kitware team that includes the original core BioGears creators (see the <a href="_f_a_q.html">FAQ</a>). 
Pulse has recently been incorporated into a number of commercial, research, and academic tools for medical simulation (see <a href="pulse-in-action.html">Pulse in action</a>)

---------

##### Validation Methodology

Models are validated using a combination of peer-reviewed publications and subject matter expertise. The validation process includes:

  - Defining key parameters for system validation
  - Performing literature reviews to gather published data in the form of waveforms, and max, min, and mean values
  - Using custom developed tools to compare data, perform analysis, and generate plots and tables of results

We validate each system at a compartment level (pressure, flow, volume, temperature, etc.). We also validate each action for a variety of system level parameters (heart rate, respiratory rate, tidal volume, blood pressure, etc.). 
We provide reports for each system in our <a href="_system_methodology.html">Methodology documentation.</a>

Below is a summary of our system and compartment level validation.

|	Key	|
|	---	|
|<span class="success"> Good agreement: correct trends, generally <10% deviation from expected </span>|
|<span class="warning"> Fair agreement / Some deviation: correct trend, generally <30% deviation from expected </span>|
|<span class="danger"> Poor agreement: incorrect trends, generally >30% deviation from expected </span>|

##### Standard Male Validation Summary

@insert ./test_results/tables/StandardMaleValidationSummaryTable.md

##### Standard Female Validation Summary

@insert ./test_results/tables/StandardFemaleValidationSummaryTable.md

---------

<div class="container">
  <div class="row">
    <h3>What can I do with the physiology engine?</h3>
    <div class="row-content">
      <div class="patient-actions">
        <div class="action col-sm-4">
          <div class="action-content">
            Define patient parameters, such as height, weight, systolic and diastolic pressure.
          </div>
        </div>
        <div class="action col-sm-4">
          <div class="action-content">
            Simulate specific chronic and/or disease states via conditions.
          </div>
        </div>
        <div class="action col-sm-4">
          <div class="action-content">
            Modify external environmental conditions (weather, submerge in water, etc.)
          </div>
        </div>
        <div class="action col-sm-4">
          <div class="action-content">
            Apply various actions (acute insults/injuries, interventions, conscious breathing, exercise, etc.) to the patient.
          </div>
        </div>
        <div class="action col-sm-4">
          <div class="action-content">
            Have patient interact with equipment models, such as an Anesthesia and/or an ECG Machine as well as an Inhaler via actions.
          </div>
        </div>
        <div class="action col-sm-4">
          <div class="action-content">
            Integrate the engine into your applications.
          </div>
        </div>
      </div>
    </div> <!-- END .row-content -->
  </div> <!-- END .row -->
  <div class="row">
    <div class="row-content">
      <h4>Available data is defined within the engine in three major ways:</h4>
      <ol class="data-definitions row">
        <li class="definition col-sm-4">
          <span>
            System data (Cardiovascular, Respiratory, etc.) that is analogous to system vitals
            <span class="examples"><strong>Examples:</strong> heart rate, oxygen consumption, mean arterial pressure, etc.</span>
          </span>
        </li>
        <li class="definition col-sm-4">
          <span>
            Compartment data
            <ul>
              <li>
                Flow, pressure, and volume related to specific region of the body or component of equipment
                <span class="examples"><strong>Examples:</strong> Cerebral Blood Flow, Right Lung Volume, Right Heart Pressure</span>
              </li>
              <li>
                Substance specific data related to a specific part of the body or component of equipment
                <span class="examples"><strong>Examples:</strong> The Extracellular concentration of succinylcholine in the brain tissue, anesthesia machine gas inlet oxygen volume fraction</span>
              </li>
            </ul>
          </span>
        </li>
        <li class="definition col-sm-4">
          <span>
            Assessments formed at the level of a clinician's report, Intended to mimic test results
            <span class="examples"><strong>Examples:</strong> Complete Blood Count</span>
          </span>
        </li>
      </ol>
    </div>
  </div>
</div> <!-- END .container -->
