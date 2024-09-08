FAQ {#FAQ}
============

<b>Is the physiology engine a game?</b><br>
No, it is an series of mathematical models (engine) that can power immersive 
learning and serious games for medical training. The 
engine can provide a realistic training experience by producing real-time 
results to trauma and treatment. A physiology engine can enhance the user experience of applications 
by providing a comprehensive physiological response to insults and interventions.

<b>What are your models and how did you validate them?</b><br>

The engine has a modular architecture to reduce costs for applications that need a physiology engine and want to develop or extend a physiology model.
The engine is a closed-loop whole body physiology model that combines physics-based lumped-parameter models and control system feedback mechanisms to model real-time system level physiologic behaviors.
Lumped-parameter models use electrical circuit analogs to represent the major physiologic systems.

Models/results are validated using a combination of peer-reviewed publications 
and subject matter expertise. The validation process includes:
- Defining key parameters for system validation
- Performing literature reviews to gather published data in the form of waveforms, and max, min, and mean values
- Using custom developed tools to compare data, perform analysis, and generate plots and tables of results

The primary purpose of model validation is to ensure that the model has an appropriate domain of validity 
given the goals of the modeling process. In the future, we plan to investigate sensitivity and uncertainty analyses to further quantify the validity of our models.
We would welcome and support, in as much as we are able, any validation or uncertainty quantification effort by the community.

We provide reports for each @ref SystemMethodology included in the physiology engine.


<b>How fast does the physiology engine run? How can I make it faster?</b><br>
The engine currently runs at about 5 to 10 times real time, depending on your machine's specifications. The included models and systems are included with the goal of creating the most complete engine possible. If your application does not require all of the existing models/systems, 
then you can strip features by modifying the source code in the same way that you would integrate a new model. Reducing the scope of the models will increase the speed of calculation.


<b>Why does it take so long to initialize the physiology engine?</b><br>
The engine represents a single, @ref PatientMethodology "variable patient".
Patient variability requires that the engine analyze the provided patient baseline values and stabilize the physiology to those values. 
This initialization can take several minutes, but once complete, the engine state can be saved to a file as protobuf binary or json.
You can then load this state and instantaneously start execution of the simulation without any initialization time.
Please consult the example in the SDK for how to take advantage of this feature and eliminate any initialization time in your application.


<b>What is the fidelity of the physiology engine?</b><br>
One definition of fidelity is "The degree to which a model or simulation represents the state and 
behavior of a real world object or the perception of a real world object, feature, condition, or chosen 
standard in a measurable or perceivable manner; a measure of the realism of a model or simulation @cite msco . " 
The validation documentation (in the @ref SystemMethodology reports) describes how well the engine 
reproduces physiology at the system level. Like the human body, the engine is a self-compensating system of 
physiological systems with outcomes based on interventions @cite pettitt2009task , and therefore can be considered high-fidelity.

Sometimes the word fidelity is used to refer to the spatial (anatomical) level of resolution of a model. 
The engine is a closed loop 
total body physiology model that combines physics-based lumped-parameter models 
and control system feedback mechanisms to model real-time system-level 
physiologic behaviors. Spatial resolution is limited by the lumped-parameter approach 
to sections of organs (what may arguably be referred to as the tissue level). However, the engine
uses an extensible architecture to promote integration with external models with varying levels of 
fidelity (resolution or granularity).


<b>What is the long-term plan for the physiology engine?</b><br>
Our team's goal is to first and foremost develop the most advanced, 
open source, whole-body engine created to date. We plan to continue advancing physiologic models and incorporating state of the art models into the physiology engine to ensure we remain at the forefront of physiologic modeling research. We also plan to continue working with the user community to ensure 
the engine becomes the standard in physiology modeling by integrating it into a variety of applications.

Our system architecture was developed in a way that will make 
the system easy to extend for new models and external interfaces. The Apache 2.0 license 
allows for both open-source and proprietary applications to promote 
widespread use across government, military, academic, and commercial markets.


<b>What kind of uncertainty quantification do you do perform in your physiology model?</b><br>
We have not performed a systematic forward propagation or inverse quantification of model uncertainty, 
nor have we conducted a formal sensitivity analysis. We plan to begin this type of analysis in the near future. However, we can quantify the numerical 
uncertainty introduced in solving the lumped-parameter fluid dynamics of the two foundation sub-models 
(@ref CardiovascularMethodology "Cardiovascular" and @ref RespiratoryMethodology "Respiratory"). The engine currently uses a bi-conjugate 
gradient method specific for sparse square systems (using the Eigen third party packages). This is an 
iterative method and we use the default tolerance for their solver, which is as close to zero as reasonable (around 1e-16). 


<b>How can I modify or extend the models in the physiology engine?</b><br>
The engine uses an extensible architecture to promote integration with external
models with varying levels of fidelity. System-level model fidelity can be 
increased or decreased by adding or removing nodes and sub-circuits.

All integration/extension will require a custom build of our @ref SourceCode.
The @ref CDM provides a standard for data interchange between models. 
The deliberate identification of data requirements must precede any model modification or addition to determine 
if an extension of the Common Data Model is required. If the existing data model is sufficient to meet your modeling needs, 
you may be able to implement changes satisfactorily just by modifying the source code for the physiologic system 
of interest. If a Common Data Model extension is necessary, modification of the source code becomes more complicated. The 
quickest and easiest way to modify the engine to meet your needs is to work with <a href="https://www.kitware.com/contact/">Kitware</a>.

We can help with requirements definition, provide development support, and/or make modifications for you.

<b>How do I ensure my changes/model are good?</b><br>
We include scenarios and their results for verification and validation. 
These results provide a baseline we can use to measure deviations to results when the code is modified.
As changes are implemented in the code base, we rerun all scenarios and compare the new results with baseline results to see how the implemented changes manifest in system data. 
Any new result that is over 2% error is marked as a failure. 
This data is used extensively to validate each system individually, as well as the combined effects of insults and interventions. See the Methodology Reports for more details. The scenarios output requests match the columns in the results file; we recommend that these scenarios remain unmodified.

<b>How can I contribute to the physiology engine?</b><br>
Take a look at our <a href="https://gitlab.kitware.com/physiology/engine/blob/master/CONTRIBUTING.md">Contribution Guide</a>.


<b>Can I contact the physiology team to work on my current or upcoming project?</b><br>
Absolutely. We always welcome new and challenging opportunities to 
work with research partners and sponsors. Please <a href="https://www.kitware.com/contact/">contact</a> us.


<b>Where did Pulse come from?</b><br>
The Pulse Physiology Engine started as a forked of the BioGears 6.1.1 Physiology Engine by members of its original team.

This fork was driven by several community needs, including:

1. Active, Publicly Available Repository<br>
The <a href="https://gitlab.kitware.com/physiology/engine">Pulse git repository</a> was created with the BioGears 6.1.1 code archive. 
The Pulse team actively develops on this repository with contributions from other community members. 
The public repository  provides a central location for the community to view and test active development, 
report issues and contribute to the Pulse Physiology Engine. This is a truly open and community-driven project.


2. Cross-Platform Build Process Improvements<br>
Pulse has refactored the build process to require only <a href="https://cmake.org/">CMake</a>, a cross-platform family of tools designed to build, test and package software. 
This removes the need for any platform and compiler customized build scripts, which has greatly simplified the process for 3rd party users
 to build and integrate Pulse into their applications. 
With this change, Pulse can easily be built with the GCC, Clang, MSVC, and Ninja compilers on any of our supported platforms (Windows, Mac, Linux, and AArch64).


3. License Updates<br>
The licensing goal of the engine is to maintain a permissive Apache 2.0 license (free to use with no restrictions)
to encourage use of the open-source software in academic, government and commercial products.
Several open-source libraries are used by the engine and have permissive licenses that do not affect the use of the engine in future products and applications.
However, the Code Synthesis XSD <a href="http://www.codesynthesis.com/licenses/gpl-2.txt">GNU General Public License</a> with <a href="http://www.codesynthesis.com/projects/xsd/FLOSSE">FLOSS exception</a>
created a significant open-source licensing limitation, which restricts any proprietary extensions of the physiology engine.
<br>
Any use of the engine, without modification, is exempt from the GPL license and does not need to be made public (open-source).
Any extensions to the engine must be made publicly available without cost, or a Proprietary License for Code Synthesis
 must be <a href="http://www.codesynthesis.com/contact/">purchased</a> for the product/application using the modified engine to comply with the license terms for Code Synthesis.
<br>
To resolve this license issue, Pulse has replaced the Xerces and Code Synthesis libraries with the <a href="https://developers.google.com/protocol-buffers/">Google Protocol Buffer</a> library. 
This library is licensed under <a href="https://opensource.org/licenses/BSD-3-Clause">BSD 3-clause</a> and allows anyone to modify the Pulse Physiology Engine in anyway without any requirement
 to make their project code base publicly available.

4. Improved Data Management and Test Suite Reporting<br>
The provided test suite executes scenarios and compares the generated results file to the "gold standard" data file for the same scenario. 
Pulse removed the SMTP requirement for viewing the test report, and instead generates a summary on disk that can be viewed by a user. 
This change reduces the required infrastructure needed to perform unit, verification and validation testing. 
The set of "gold standard" data files  for the physiology engine is very large (~500MB). 
Keeping these files in the git repository can easily bloat the repository. 
Pulse has addressed this issue by hosting all verification sets in <a href="https://data.kitware.com/#collection/59849c788d777f7d33e9c084/folder/598c95d88d777f7d33e9c1fa">a publicly available data server</a>, 
associating different versions to the git repository history though a collection of hash files in the git repository. 
CMake is then used to provide management between the data server and the specific code version from the git repository. 
This separation provides a repository with a small footprint to allow for quick pulling and branching, 
while maintaining a historical associations with the large data set needed for testing.



