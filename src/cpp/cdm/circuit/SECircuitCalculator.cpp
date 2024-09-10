/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/circuit/SECircuitCalculator.h"
#include "cdm/circuit/SECircuit.h"
#include "cdm/circuit/SECircuitNode.h"
#include "cdm/circuit/SECircuitPath.h"
#include "cdm/properties/SEScalar.h"
#include "cdm/utils/GeneralMath.h"
#include "cdm/utils/TimingProfile.h"

PUSH_EIGEN_WARNINGS
#include "Eigen/Dense"
#include "Eigen/SparseLU"
#include "Eigen/SparseCore"
//#include "Eigen/SparseCholesky"
#include "Eigen/IterativeLinearSolvers"
#include "Eigen/SparseQR"
POP_EIGEN_WARNINGS

#include <numeric>
#include <bitset>

#include <iostream>
#include <fstream>
using namespace std;

class eigen
{
public:
  //Ax=b
  Eigen::MatrixXd AMatrix; //A
  Eigen::VectorXd xVector; //x
  Eigen::VectorXd bVector; //b
};

// Note this must match the enum values
template<> char const* enumStrings<EigenCircuitSolver>::values[] = { "Direct", "PartialPivLu", "FullPivLu", "JacobiSvd", "HouseholderQr", "Ldlt", "Llt", "SparseLU", "SparseQR", "BiCGSTAB", "ConjugateGradient" };
char const* EigenCircuitSolver::Value(size_t idx)
{
  return enumStrings<EigenCircuitSolver>::values[idx];
}


//#define VERBOSE
#define OPEN_RESISTANCE 1e100
#define CLOSED_RESISTANCE 1e-100

template<CIRCUIT_CALCULATOR_TEMPLATE>
SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::SECircuitCalculator(const CapacitanceUnit& c, const FluxUnit& f, const InductanceUnit& i, const PotentialUnit& p, const QuantityUnit& q, const ResistanceUnit& r, Logger* logger) : Loggable(logger),
m_CapacitanceUnit(c), m_FluxUnit(f), m_InductanceUnit(i), m_PotentialUnit(p), m_QuantityUnit(q), m_ResistanceUnit(r)
{
  m_currentTime_s = 0;
  m_solver.set(EigenCircuitSolver::SparseLU);

  //Make sure the base units are compatible
  if (GeneralMath::PercentTolerance(m_ResistanceUnit.GetBigness(), (m_PotentialUnit.GetBigness() / m_FluxUnit.GetBigness())) > 0.001 ||
    GeneralMath::PercentTolerance(m_CapacitanceUnit.GetBigness(), (m_QuantityUnit.GetBigness() / m_PotentialUnit.GetBigness())) > 0.001 ||
    GeneralMath::PercentTolerance(m_InductanceUnit.GetBigness(), (m_ResistanceUnit.GetBigness() * m_QuantityUnit.GetBigness() / m_FluxUnit.GetBigness())) > 0.001)
  {
    m_ss << "Incompatible base units." << m_CapacitanceUnit << "(" << c.GetBigness() << ") " << m_FluxUnit << "(" << f.GetBigness() << ") " << m_InductanceUnit << "(" << i.GetBigness() << ") " << m_PotentialUnit << "(" << p.GetBigness() << ") " << m_QuantityUnit << "(" << q.GetBigness() << ") " << m_ResistanceUnit << "(" << r.GetBigness() << ") ";
    Fatal(m_ss);
  }

  _eigen = new eigen();
}
template<CIRCUIT_CALCULATOR_TEMPLATE>
SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::~SECircuitCalculator()
{
  delete _eigen;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the Next circuit values.
///
/// \param  Circuit The circuit in question.
///
/// \details
/// Take the Element values and calculate the Node and Path variables for the Next time-step.
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::Process(CircuitType& circuit, double timeStep_s)
{
  m_circuit = &circuit;
  m_dT_s = timeStep_s;
  m_valveStates.clear();
  m_currentTime_s += timeStep_s;

  //Reset all Polarized Elements to be shorted.
  for (PathType* p : circuit.GetPolarizedElementPaths())
  {
    if (p->HasNextPolarizedState())
      p->SetNextPolarizedState(eGate::Closed);
  }

  //When we parse everything into our Ax=b matrices/vectors for the linear solver,
  //the address (row,column) is based on indexes.
  // The circuit has a method for getting the A matrix index for all nodes except the reference node
  //We'll solve circuits with switches using Assumed Valve (i.e. Diode) States model by thinking of them as switches
  //and checking the resulting Flux and Potential difference.
  //We'll keep looping until we've either found a solution or determined that it cannot be solved in the current configuration.
#ifdef VERBOSE
  int i = 0;
#endif
  bool success = false;
  do
  {
#ifdef VERBOSE
    i++;
#endif
    //We solve for the unknown circuit values this time-step by using Modified Nodal Analysis and linear algebra.
    //All of the source (i.e. Potential and Flux) values are known, as well as all element (i.e. Resistance, Compliance, Inertance, Switch on/off, Valve direction) values.
    //We'll solve for all of the circuit's node potentials and Potential Source Fluxes simultaneously using an error minimization numerical solver.
    //Then we calculate the fluxes and voltages based on those Potentials and Element values.    
    ParseIn();
    success = Solve();
    if (success)
    {
      ParseOut();
      CalculateFluxes();
    }
  } while (!CheckAndModifyValves(success));
#ifdef VERBOSE
  std::cout << "Number of Valve Loops = " << i << std::endl;
#endif

  if (!success)
  {
    double absolute_error = (_eigen->AMatrix * _eigen->xVector - _eigen->bVector).norm();
    double relative_error = absolute_error / _eigen->bVector.norm();

    std::stringstream ss;
    ss << "The solver was unable to determine a solution for the circuit with the attempted combination of valves and/or polarized elements. "
      << "Relative error = " << relative_error
      << ". Absolute error = " << absolute_error;
    Fatal(ss);
  }

  CalculateQuantities();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Set up the equations to solve the circuit
///
/// \details
/// Define the state-space system, by creating the A matrix and b solution vector (Ax=b).
/// We do all of the Modified Nodal Analysis setup steps all at once below. There are (using electrical terminology):
/// 1) Select a reference node
/// 2) Assign a name to the current through each voltage source.  We will use the convention that the current fluxes from the positive node to the negative node of the source.
/// 3) Apply Kirchoff's current law to each node.  We will take currents out of the node to be positive.
/// 4) Write an equation for the voltage each voltage source.
/// 5) Solve the system of n-1 unknowns.
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::ParseIn()
{
  // Check the Reference Node
  if (!m_circuit->HasReferenceNode())
  {
    Fatal("Circuit must have at least 1 reference node");
  }

  // Assign calculator index to the nodes in the circuit
  int numVars = 0;
  for (NodeType* n : m_circuit->GetNodes())
  {
    //There should never be a next potential value set on a node
    //  //Initializing a compliance "charge" is done on the current potential value
    //  //Potential sources are defined on a path
    //  if (n->HasNextPotential() && n!=GetReferenceNode())
    //  {
    //    m_ss << "You cannot set a potential value without using a path potential source.  The NextPotential value will be ignored and overwritten for Node " << n->GetName();
    //    Warning(m_ss);
    //  }
    if (!n->IsReferenceNode())
    {
      n->SetCalculatorIndex(numVars);
      numVars++;
    }
    else
    {
      n->SetCalculatorIndex(-1);
    }
  }

  size_t numRefNodes = 0;
  {// Create some scope for r
    NodeType* r = nullptr;
    for (NodeType* ref : m_circuit->GetNodes())
    {
      if (!ref->IsReferenceNode())
      {
        continue;
      }
      numRefNodes++;

      if (!ref->HasPotential() && !ref->HasNextPotential())
      {
        Warning("Reference potential is not defined - setting it to 0.");
        ValueOverride<PotentialUnit>(ref->GetNextPotential(), 0, m_PotentialUnit);
        Override<PotentialUnit>(ref->GetNextPotential(), ref->GetPotential());
      }
      else if (!ref->HasPotential())
        Override<PotentialUnit>(ref->GetNextPotential(), ref->GetPotential());
      if (!ref->HasNextPotential())
        Override<PotentialUnit>(ref->GetPotential(), ref->GetNextPotential());
      if (r == nullptr)
        r = ref;
      else if (!r->GetPotential().Equals(r->GetPotential()))
      {
        Fatal("Multiple Reference Potentials must be equal");
      }
    }
  }

  m_potentialSources.clear();
  for (PathType* p : m_circuit->GetPaths())
  { 
    //Set aside the potential sources, since the Flux through them will be directly solved by adding them to the bottom of the matrix.
    //We have to do this outside of the KCL loop below because we only want to account for each one once.
    if (p->HasNextPotentialSource() ||
      (p->NumberOfNextElements() < 1 && !p->IsPartOfBlackBox()) ||
      (p->HasNextValve() && p->GetNextValve() == eGate::Closed) ||
      (p->HasNextSwitch() && p->GetNextSwitch() == eGate::Closed))
    {
      m_potentialSources[p] = numVars++;
    }
  }
  
  //Black boxes with imposed potentials are like potential sources to ground, but without a path defined
  m_blackBoxPotentialSources.clear();
  for (NodeType* n : m_circuit->GetNodes())
  {
    if (n->IsPartOfBlackBox())
    {
      if (n->IsPotentialImposed())
      {
        m_blackBoxPotentialSources[n] = numVars++;
      }
    }
  }

  //Set the size of the matrix and initialize all elements to zero - we'll populate it later.
  //subtract the known reference - we don't need to solve for that potential
  //(and it has to be known or we'll have too many unknowns).  
  _eigen->AMatrix = Eigen::MatrixXd::Zero(numVars, numVars);
  //Set the right side b vector
  //All zeros - we'll modify the values later
  _eigen->bVector = Eigen::VectorXd::Zero(numVars);

  //Build the A matrix to solve using linear algebra
  //We basically walk through all of the nodes and do the KCL for each
  //The A matrix will have all of the left side values (variable multipliers) from the KCL equation
  //The b matrix will have all of the right side values (known values) from the KCL equation
  //Variables used in the loop
  double dStartingCompliance = 0.0;
  for (NodeType* n : m_circuit->GetNodes())
  {
    //Sum of the fluxes at each node is 0

    //Skip known reference node
    if (n->IsReferenceNode())
    {
      continue;
    }

    if (n->IsBlackBoxMiddle())
    {
      //Handle the black box middle node
      //We'll either explicitly set the potential value or just make it the average of the source and target
      //If we don't do this, it creates a singular matrix (zero columns) and will take longer to solve
      _eigen->AMatrix(n->GetCalculatorIndex(), n->GetCalculatorIndex()) = 1.0;
      if (n->IsPotentialImposed())
      {
        _eigen->bVector(n->GetCalculatorIndex()) = n->GetNextPotential().GetValue(m_PotentialUnit);
      }
      else
      {
        //Not Ref: BB - Source/2 - Target/2 = 0
        //Source Ref: BB - Target/2 = 0
        //Target Ref: BB - Source/2 = 0
        if (!n->GetBlackBoxSourceNode()->IsReferenceNode())
        {
          _eigen->AMatrix(n->GetCalculatorIndex(), n->GetBlackBoxSourceNode()->GetCalculatorIndex()) = -0.5;
        }

        if (!n->GetBlackBoxTargetNode()->IsReferenceNode())
        {
          _eigen->AMatrix(n->GetCalculatorIndex(), n->GetBlackBoxTargetNode()->GetCalculatorIndex()) = -0.5;
        }
      }

      //Phantom path to ground needed to determine flux into/out of the black box
      //Out of node is positive
      NodeType* sourceNode = n->GetBlackBoxSourceNode();
      NodeType* targetNode = n->GetBlackBoxTargetNode();
      PathType* sourcePath = n->GetBlackBoxSourcePath();
      PathType* targetPath = n->GetBlackBoxTargetPath();

      if (sourceNode->IsPotentialImposed())
      {
        if (sourceNode->IsReferenceNode())
        {
          Fatal("Black boxes cannot set the reference node potential : " + sourceNode->GetName());
        }
        else
        {
          //Potential to be set and flux to be directly solved, like potential sources
          double sign = (sourceNode == &sourcePath->GetSourceNode()) ? -1 : 1;
          _eigen->AMatrix(sourceNode->GetCalculatorIndex(), m_blackBoxPotentialSources[sourceNode]) += sign;
          _eigen->AMatrix(m_blackBoxPotentialSources[sourceNode], sourceNode->GetCalculatorIndex()) += 1.0;
          _eigen->bVector(m_blackBoxPotentialSources[sourceNode]) = sourceNode->GetNextPotential().GetValue(m_PotentialUnit);
        }
      }

      if (targetNode->IsPotentialImposed())
      {
        if (targetNode->IsReferenceNode())
        {
          Fatal("Black boxes cannot set the reference node potential : " + targetNode->GetName());
        }
        else
        {
          //Potential to be set and flux to be directly solved, like potential sources
          double sign = (targetNode == &targetPath->GetSourceNode()) ? -1 : 1;
          _eigen->AMatrix(targetNode->GetCalculatorIndex(), m_blackBoxPotentialSources[targetNode]) += sign;
          _eigen->AMatrix(m_blackBoxPotentialSources[targetNode], targetNode->GetCalculatorIndex()) += 1.0;
          _eigen->bVector(m_blackBoxPotentialSources[targetNode]) = targetNode->GetNextPotential().GetValue(m_PotentialUnit);
        }
      }

      continue;
    }

    for (PathType* p : *m_circuit->GetConnectedPaths(*n))
    {
      NodeType* nSrc = &p->GetSourceNode();
      NodeType* nTgt = &p->GetTargetNode();

      if (p->HasNextPolarizedState() && p->GetNextPolarizedState() == eGate::Open)
      { 
        //Polarized elements that are open are done exactly the same as a open switch.
        //We'll check to see if the resulting potential difference is valid later.
        //Model as an open switch
        double dMultiplier = 1.0 / OPEN_RESISTANCE;
        PopulateAMatrix(*n, *p, dMultiplier);
        //Don't allow to function normally - i.e. continue to the next path
        continue;
      }

      //Each Path has only one Element (or none at all) and each type is handled differently.
      //The variables in the x vector are the unknown Node Potentials and Fluxes for Potential Sources.
      if (p->IsPartOfBlackBox())
      {
        //Currents out of the node are assumed positive and the sign is switched when moving from the left side of the equation to the right.
        //Therefore, out of the Node we're current analyzing (i.e. Source) reverses the sign when it goes into the right side vector.
        if (p->IsFluxImposed())
        {
          double flux = p->GetNextFlux().GetValue(m_FluxUnit);
          int sign = (n == nSrc) ? -1 : 1;
          _eigen->bVector(n->GetCalculatorIndex()) += (sign * flux);
        }
      }
      else if (p->HasNextSwitch())
      {
        if (p->GetNextSwitch() == eGate::Open)
        {
          //Model as a resistor with a ridiculously high resistance (basically an open circuit)
          double dMultiplier = 1.0 / OPEN_RESISTANCE;
          PopulateAMatrix(*n, *p, dMultiplier);
        }
        else
        {
          //Model as a zero potential source
          PopulateAMatrix(*n, *p, 1, true);
        }
      }
      else if (p->HasNextResistance())
      {
        double r = p->GetNextResistance().GetValue(m_ResistanceUnit);
        if (r <= 0.0)
        {
          Fatal("Resistance cannot be negative or zero : " + p->GetName());
        }
        double dMultiplier = 1.0 / r;
        PopulateAMatrix(*n, *p, dMultiplier);
      }
      else if (p->HasNextCapacitance())
      {
        //Capacitors use initial voltage
        if (!nSrc->HasPotential())
        {
          //Initial source potential is not defined, assume it's the same as the reference
          ValueOverride<PotentialUnit>(nSrc->GetPotential(), 0.0, m_PotentialUnit);
          Warning("Initial capacitance source potential is not defined for " + nSrc->GetName() + ", assuming it is the reference potential.");
        }

        if (!nTgt->HasPotential())
        {
          //Initial target potential is not defined, assume it's the same as the reference
          ValueOverride<PotentialUnit>(nTgt->GetPotential(), 0.0, m_PotentialUnit);
          Warning("Initial capacitance target potential is not defined for " + nTgt->GetName() + ", assuming it is the reference potential.");
        }

        //We need to handle the first time through when there is no Current value
        if (p->HasCapacitance())
        {
          dStartingCompliance = p->GetCapacitance().GetValue(m_CapacitanceUnit);
        }
        else
        {
          dStartingCompliance = p->GetNextCapacitance().GetValue(m_CapacitanceUnit);
          if (dStartingCompliance < 0.0)
          {
            Fatal("Capacitance is negative for " + p->GetName());
          }
        }

        //Add left side of the equation
        double dMultiplier = p->GetNextCapacitance().GetValue(m_CapacitanceUnit) / m_dT_s;
        PopulateAMatrix(*n, *p, dMultiplier);

        //Add the known initial time values to the right side of the equation
        //C(0)/dt*(dV(0))
        dMultiplier = dStartingCompliance / m_dT_s;

        double dLastPotentialDiff = 0;
        if (n == nSrc)
          dLastPotentialDiff = nSrc->GetPotential().GetValue(m_PotentialUnit) - nTgt->GetPotential().GetValue(m_PotentialUnit);
        else if (n == nTgt)
          dLastPotentialDiff = nTgt->GetPotential().GetValue(m_PotentialUnit) - nSrc->GetPotential().GetValue(m_PotentialUnit);

        _eigen->bVector(n->GetCalculatorIndex()) += (dMultiplier * dLastPotentialDiff);
      }
      else if (p->HasNextInductance())
      {
        //Inductors use initial current
        if (!p->HasFlux())
        {
          //Initial flux is not defined, assume it's 0
          ValueOverride<FluxUnit>(p->GetFlux(), 0.0, m_FluxUnit);
          Warning("Initial inductance is not defined for " + p->GetName() + ", assuming it is 0.");
        }

        if (!nSrc->HasPotential())
        {
          //Initial source potential is not defined, assume it's the same as the reference
          ValueOverride<PotentialUnit>(nSrc->GetPotential(), 0.0, m_PotentialUnit);
          Warning("Initial inductance source potential is not defined " + nSrc->GetName() + ", assuming it is the reference potential.");
        }
        if (!nTgt->HasPotential())
        {
          //Initial source potential is not defined, assume it's the same as the reference
          ValueOverride<PotentialUnit>(nTgt->GetPotential(), 0.0, m_PotentialUnit);
          Warning("Initial inductance target potential is not defined " + nTgt->GetName() + ", assuming it is the reference potential.");
        }

        double pInductance = p->GetNextInductance().GetValue(m_InductanceUnit);
        if (pInductance < 0.0)
        {
          Fatal("Inductance is negative for " + p->GetName());
        }

        double dMultiplier = m_dT_s / (2 * pInductance);
        PopulateAMatrix(*n, *p, dMultiplier);

        double dLastFlux = 0.0;
        double dLastPotentialDiff = 0.0;
        if (n == nSrc)
        {
          dLastFlux = -1.0 * p->GetFlux().GetValue(m_FluxUnit);
          dLastPotentialDiff = n->GetPotential().GetValue(m_PotentialUnit) - nTgt->GetPotential().GetValue(m_PotentialUnit);
        }
        else if (n == nTgt)
        {
          dLastFlux = p->GetFlux().GetValue(m_FluxUnit);
          dLastPotentialDiff = n->GetPotential().GetValue(m_PotentialUnit) - nSrc->GetPotential().GetValue(m_PotentialUnit);
        }
        _eigen->bVector(n->GetCalculatorIndex()) += dLastFlux - (dMultiplier * dLastPotentialDiff);
      }
      else if (p->HasNextValve())
      {
        //Valves are done exactly the same as switches.
        //We'll check to see if the resulting flux and potential difference is valid later.
        if (p->GetNextValve() == eGate::Open)
        {
          //Model as a resistor with a ridiculously high resistance (basically an open circuit)
          double dMultiplier = 1.0 / OPEN_RESISTANCE;
          PopulateAMatrix(*n, *p, dMultiplier);
        }
        else
        {
          //Model as a zero potential source
          PopulateAMatrix(*n, *p, 1, true);
        }
      }
      else if (p->HasNextFluxSource())
      {
        //Currents out of the node are assumed positive and the sign is switched when moving from the left side of the equation to the right.
        //Therefore, out of the Node we're current analyzing (i.e. Source) reverses the sign when it goes into the right side vector.
        double sign = (n == nSrc) ? -1 : 1;
        double dFlux = p->GetNextFluxSource().GetValue(m_FluxUnit);
        _eigen->bVector(n->GetCalculatorIndex()) += (sign * dFlux);
      }
      else if (p->HasNextPotentialSource())
      {
        PopulateAMatrix(*n, *p, 1, true);
      }
      else
      {
        //No element
        //Model as a 0v voltage source
        PopulateAMatrix(*n, *p, 1, true);
      }

#ifdef VERBOSE
      Verbose("NodeLoop");
#endif
    }
  }

  ParseInPotentialSources();

#ifdef VERBOSE
  Verbose("PostPotentialSources");
#endif

#ifdef VERBOSE
  Verbose("PostBlackBoxNodes");
#endif
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Solve for unknowns.
///
/// \details
/// Use the Eigen solver to find the unknowns.
/// Make sure the solution is correct.
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
bool SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::Solve()
{
#ifdef VERBOSE
  Verbose("PreSolve");
#endif

  bool sparseFailed = false;
  switch (m_solver.value())
  {
    //Use the slow direct-solve method
  case EigenCircuitSolver::Direct:
    _eigen->xVector = _eigen->AMatrix.inverse() * _eigen->bVector;
    break;
    //Dense methods
  case EigenCircuitSolver::PartialPivLu:
    _eigen->xVector = _eigen->AMatrix.partialPivLu().solve(_eigen->bVector);
    break;
  case EigenCircuitSolver::FullPivLu:
    _eigen->xVector = _eigen->AMatrix.fullPivLu().solve(_eigen->bVector);
    break;
  case EigenCircuitSolver::JacobiSvd:
    _eigen->xVector = _eigen->AMatrix.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(_eigen->bVector);
    break;
  case EigenCircuitSolver::HouseholderQr:
    _eigen->xVector = _eigen->AMatrix.householderQr().solve(_eigen->bVector);
    break;
  case EigenCircuitSolver::Ldlt:
    _eigen->xVector = _eigen->AMatrix.ldlt().solve(_eigen->bVector);
    break;
  case EigenCircuitSolver::Llt:
    _eigen->xVector = _eigen->AMatrix.llt().solve(_eigen->bVector);
    break;
  //Sparse methods
  case EigenCircuitSolver::SparseLU:
  {
    Eigen::SparseMatrix<double> sparse = _eigen->AMatrix.sparseView();
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    solver.compute(sparse);

    if (solver.info() != Eigen::Success)
    {
      //Decomposition failed
      sparseFailed = true;
      break;
    }

    _eigen->xVector = solver.solve(_eigen->bVector);
    if (solver.info() != Eigen::Success)
    {
      //Solving failed
      sparseFailed = true;
    }
    break;
  }
  case EigenCircuitSolver::SparseQR:
  {
    Eigen::SparseMatrix<double> sparse = _eigen->AMatrix.sparseView();
    //sparse.makeCompressed();
    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
    solver.compute(sparse);

    if (solver.info() != Eigen::Success)
    {
      //Decomposition failed
      sparseFailed = true;
      break;
    }

    _eigen->xVector = solver.solve(_eigen->bVector);
    if (solver.info() != Eigen::Success)
    {
      //Solving failed
      sparseFailed = true;
    }
    break;
  }
  case EigenCircuitSolver::BiCGSTAB:
  {
    if (_eigen->xVector.rows() != _eigen->AMatrix.rows())
    {
      _eigen->xVector = Eigen::VectorXd::Zero(_eigen->AMatrix.rows());
    }

    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
    //Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IncompleteLUT<double>> solver;
    solver.compute(_eigen->AMatrix.sparseView());

    if (solver.info() != Eigen::Success)
    {
      //Decomposition failed
      sparseFailed = true;
      break;
    }

    _eigen->xVector = solver.solve(_eigen->bVector);
    //_eigen->xVector = solver.solveWithGuess(_eigen->bVector, _eigen->xVector).eval();
    if (solver.info() != Eigen::Success)
    {
      //Solving failed
      sparseFailed = true;
    }
    break;
  }
  case EigenCircuitSolver::ConjugateGradient:
  {
    Eigen::SparseMatrix<double> sparse = _eigen->AMatrix.sparseView();
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;
    solver.compute(sparse);

    if (solver.info() != Eigen::Success)
    {
      //Decomposition failed
      sparseFailed = true;
      break;
    }

    _eigen->xVector = solver.solve(_eigen->bVector);
    if (solver.info() != Eigen::Success)
    {
      //Solving failed
      sparseFailed = true;
    }
    break;
  }
  default:
  {
    Warning("Invalid solver type requested; using backup method");
    sparseFailed = true;
  }
  };

  double errorThreshold = 1e-6;
  if (sparseFailed || !(_eigen->AMatrix*_eigen->xVector).isApprox(_eigen->bVector, errorThreshold))
  {
    //The faster sparse solver should almost always work
    //If it didn't, do it dense and make sure we get an answer (if possible)

    _eigen->xVector = _eigen->AMatrix.fullPivLu().solve(_eigen->bVector);

    if (!(_eigen->AMatrix * _eigen->xVector).isApprox(_eigen->bVector, errorThreshold))
    {
      //If that didn't work, solve it directly

      _eigen->xVector = _eigen->AMatrix.inverse() * _eigen->bVector;

      if (!(_eigen->AMatrix * _eigen->xVector).isApprox(_eigen->bVector, errorThreshold))
      {
        //That still didn't work, so we'll just try the next combination of valves and/or polarized elements by returning false

#ifdef VERBOSE
        double absolute_error = (_eigen->AMatrix * _eigen->xVector - _eigen->bVector).norm();
        double relative_error = absolute_error / _eigen->bVector.norm();

        std::stringstream ss;
        ss << "The solver was unable to determine a solution for the circuit with the attempted combination of valves and/or polarized elements. "
          << "Relative error = " << relative_error
          << ". Absolute error = " << absolute_error;
        Warning(ss);
#endif
        return false;
      }
    }
  }
#ifdef VERBOSE
  Verbose("PostSolve");
#endif

  if (_eigen->AMatrix.rows() != _eigen->AMatrix.cols() ||
    _eigen->AMatrix.rows() != _eigen->xVector.rows() ||
    _eigen->AMatrix.rows() != _eigen->bVector.rows())
  {
    Fatal("The solver was unable to determine a solution for the circuit due to a matrix size mismatch.");
  }

  return true;
}


//--------------------------------------------------------------------------------------------------
/// \brief
/// Assign the Solver solution vector values to the appropriate places.
///
/// \details
/// Set all Node Potentials.
/// Set Potential Source Fluxes.
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::ParseOut()
{
  bool refNodeExists = false;
  for (NodeType* n : m_circuit->GetNodes())
  {
    if (n->IsReferenceNode())
    {
      if (n->HasNextPotential())
        m_refPotential = n->GetNextPotential().GetValue(m_PotentialUnit);
      else
        m_refPotential = n->GetPotential().GetValue(m_PotentialUnit);
      refNodeExists = true;
      break;
    }
  }

  if (!refNodeExists)
  {
    Fatal("No reference node in the circuit ");
  }

  for (NodeType* n : m_circuit->GetNodes())
  {
    if (!n->IsReferenceNode())
    {
      //Add the reference potential
      //For the calculations, we assume the reference potential is zero
      //When it's not zero, all potentials are just offset by that amount
      double potential = _eigen->xVector(n->GetCalculatorIndex()) + m_refPotential;
      ValueOverride<PotentialUnit>(n->GetNextPotential(), potential, m_PotentialUnit);
    }
  }

  for (auto itr : m_potentialSources)
  {
    //The potential source fluxes are all in order after the Node Potentials.
    double flux = _eigen->xVector(itr.second);
    //We define potential sources as the difference in potential from source to target.
    //Therefore, a positive value means the target potential is greater than the source (and reverse for negative values).
    //This means flux would go Target to Source (high to low potential), and we need to reverse the sign.
    ValueOverride<FluxUnit>(itr.first->GetNextFlux(), -flux, m_FluxUnit);
  }

  for (auto itr : m_blackBoxPotentialSources)
  {
    //The potential source fluxes are all in order after the Node Potentials.
    double flux = _eigen->xVector(itr.second);
    NodeType* n = itr.first;

    if (n->IsBlackBoxMiddle())
    {
      continue;
    }

    for (PathType* p : m_circuit->GetPaths())
    {
      if (!p->IsPartOfBlackBox() || p->IsFluxImposed())
      {
        continue;
      }

      NodeType* sourceNode = &p->GetSourceNode();
      NodeType* targetNode = &p->GetTargetNode();

      if (n == sourceNode || n == targetNode)
      {
        ValueOverride<FluxUnit>(p->GetNextFlux(), -flux, m_FluxUnit);
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculate and set all Path Flux values.
///
/// \details
/// Set fluxes that were given for Flux Sources.
/// Set fluxes that were directly solved for Potential Sources.
/// Calculate and set Fluxes for Paths with other Elements.
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::CalculateFluxes()
{
  //Calculate fluxes (currents)
  //Note: fluxes use source->target convention for positive flux, so the source potential needs to be larger than the target potential for positive flux
  for (PathType* p : m_circuit->GetPaths())
  {
    if (p->HasNextFluxSource())
    {
      Override<FluxUnit>(p->GetNextFluxSource(), p->GetNextFlux());
    } 
    else if ((p->HasNextSwitch() && p->GetNextSwitch() == eGate::Open) ||
      (p->HasNextValve() && p->GetNextValve() == eGate::Open))
    {
      ValueOverride<FluxUnit>(p->GetNextFlux(), 0, m_FluxUnit);
    }
    else if (p->HasNextCapacitance())
    {
      //Positive flux is from source to target (i.e. SourcePotential > TargetPotential = Positive Flux)
      //dStartingCompliance is at time = T
      double dStartingCompliance = 0.0;
      if (p->HasCapacitance())
        dStartingCompliance = p->GetCapacitance().GetValue(m_CapacitanceUnit);
      else
        dStartingCompliance = p->GetNextCapacitance().GetValue(m_CapacitanceUnit);
      //dEndingCompliance is at time = T + deltaT  
      double dEndingCompliance = p->GetNextCapacitance().GetValue(m_CapacitanceUnit);
      //dStartingPotentialDiff is at time = T
      double dStartingPotentialDiff = p->GetSourceNode().GetPotential().GetValue(m_PotentialUnit) - p->GetTargetNode().GetPotential().GetValue(m_PotentialUnit);
      //dStartingPotentialDiff is at time = T + deltaT  
      double dEndingPotentialDiff = p->GetSourceNode().GetNextPotential().GetValue(m_PotentialUnit) - p->GetTargetNode().GetNextPotential().GetValue(m_PotentialUnit);

      if (p->HasPolarizedState() && p->GetPolarizedState() == eGate::Open)
      {
        //If this was a shorted polarized element last time-step, we need to make the starting difference zero
        //Otherwise, it will possibly think it was already charged to a certain point
        dStartingPotentialDiff = 0.0;
      }
      if (p->HasNextPolarizedState() && p->GetNextPolarizedState() == eGate::Open)
      {
        //If it is currently a shorted polarized element, we need to make the ending difference zero
        //This will make it go to the non-charged quantity
        dEndingPotentialDiff = 0.0;
      }

      double dQuantityIncrement = dEndingCompliance * dEndingPotentialDiff - dStartingCompliance * dStartingPotentialDiff;
      double dFlux = dQuantityIncrement / m_dT_s;
      ValueOverride<FluxUnit>(p->GetNextFlux(), dFlux, m_FluxUnit);
    }
    else if (p->HasNextPolarizedState() && p->GetNextPolarizedState() == eGate::Open)
    {
      //This needs to be after capacitance because that's a special case
      ValueOverride<FluxUnit>(p->GetNextFlux(), 0, m_FluxUnit);
    }
    else if (p->HasNextResistance())
    {
      //I = V/R
      double dResistance;
      dResistance = p->GetNextResistance().GetValue(m_ResistanceUnit);
      double dPotentialDiff = p->GetSourceNode().GetNextPotential().GetValue(m_PotentialUnit) - p->GetTargetNode().GetNextPotential().GetValue(m_PotentialUnit);
      double dFlux = dPotentialDiff / dResistance;
      ValueOverride<FluxUnit>(p->GetNextFlux(), dFlux, m_FluxUnit);
    }

    else if (p->HasNextInductance())
    {
      //V = L*dI/dt
      //I = dt/L*V+I(0)
      double dPotentialDiff = p->GetTargetNode().GetNextPotential().GetValue(m_PotentialUnit) - p->GetSourceNode().GetNextPotential().GetValue(m_PotentialUnit);
      double dInitialPotentialDiff = p->GetTargetNode().GetPotential().GetValue(m_PotentialUnit) - p->GetSourceNode().GetPotential().GetValue(m_PotentialUnit);
      double dInitialFlux = -1.0 * p->GetFlux().GetValue(m_FluxUnit);
      double dFlux = m_dT_s / (2 * p->GetNextInductance().GetValue(m_InductanceUnit)) * (dPotentialDiff + dInitialPotentialDiff) + dInitialFlux;
      ValueOverride<FluxUnit>(p->GetNextFlux(), -dFlux, m_FluxUnit);
    }
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculate and set all Path Quantities.
///
/// \details
/// Calculate and set the Quantitys for all Paths with Compliance Elements.
/// You need a compliance because quantity can't change in a rigid pipe.
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::CalculateQuantities()
{
  //Calculate quantity changes due to compliance.
  //You need a compliance because quantity can't change in a rigid pipe.

  //Quantities are only calculated for paths with a compliance, and each model is responsible for populating node quantities based on those values.
  for (PathType* p : m_circuit->GetPaths())
  {
    // Calculate Quantities
    if (p->HasNextCapacitance())
    {
      //Charge is analogues to quantity
      double flux = p->GetNextFlux().GetValue(m_FluxUnit);
      double dQuantityIncrement = flux * m_dT_s;

      if (p->GetSourceNode().HasNextQuantity() && !p->GetSourceNode().GetNextQuantity().IsInfinity())
      {
        IncrementOverride<QuantityUnit>(p->GetSourceNode().GetNextQuantity(), dQuantityIncrement, m_QuantityUnit);
      }

      if (p->GetTargetNode().HasNextQuantity() && !p->GetTargetNode().GetNextQuantity().IsInfinity())
      {
        IncrementOverride<QuantityUnit>(p->GetTargetNode().GetNextQuantity(), -dQuantityIncrement, m_QuantityUnit);
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Check if the solutions meets all Valve criteria, and modify a Valve state if not.
///
/// \return true if the solution meets all Valve criteria, false if not
///
/// \details
/// The solution passes if:
/// 1) There is a path with a valve and flux in the wrong direction.
/// 2) Check reverse bias (i.e. voltage must be positive at the cathode).
/// Look at the potentials to ensure flux would be going against the diode if the "switch" (ideal diode is modeled as a switch) was closed.
/// Since flux goes from low to high potential, the potential at the Target must be higher or equal to the Source for negative flux (against valve).
/// 
/// If it does not pass, change the state (Open/Close) of the first Valve that fails.
/// The circuit will be re-solved with the new Valve state, and will iterate until a solution is found.
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
bool SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::CheckAndModifyValves(bool solverPassed)
{
  if (m_circuit->GetPolarizedElementPaths().empty() && m_circuit->GetValvePaths().empty())
    return true;//There aren't any valves to worry about

  if (m_valveStates.size() >= ((uint64_t)(pow(2.0, int(m_valveStates.size())))))
  {
    Fatal("The combination of valves precludes the circuit from being solved.");
  }

  for (PathType* p : m_circuit->GetValvePaths())
  {
    if (!solverPassed
      ||
      (p->GetNextValve() == eGate::Closed &&
      p->GetNextFlux().GetValue(m_FluxUnit) < -ZERO_APPROX)
      ||
      (p->GetNextValve() == eGate::Open &&
      (p->GetSourceNode().GetNextPotential().GetValue(m_PotentialUnit) -
        p->GetTargetNode().GetNextPotential().GetValue(m_PotentialUnit)) > ZERO_APPROX))
    {
      p->FlipNextValve();
      if (IsCurrentValveStateUnique())
      {
        return false;
      }
      p->FlipNextValve();// Not unique, flip it back
    }
  }

  for (PathType* p : m_circuit->GetPolarizedElementPaths())
  {
    if (!solverPassed
      || 
      (p->GetNextPolarizedState() == eGate::Closed &&
      (p->GetSourceNode().GetNextPotential().GetValue(m_PotentialUnit) -
        p->GetTargetNode().GetNextPotential().GetValue(m_PotentialUnit)) < -ZERO_APPROX))
    {
      p->FlipNextPolarizedState();
      if (IsCurrentValveStateUnique())
      {
        return false;
      }
      p->FlipNextPolarizedState();// Not unique, flip it back
    }
  }
  return true;
}
//--------------------------------------------------------------------------------------------------
/// \brief
/// Check if the current Valve state combination has already been analyzed.
///
/// \return true if the Valve combination is unique, false if it is not.
///
/// \details
/// Loop through all of the previously analyzed Valve combinations this time-step and check if the current combination is the same.
/// If this combination is unique, store it in a vector for the next go-round to check.
/// Valve combinations are stored as integers that use their index and state (Open = 0, Closed = 1) to calculate a unique value for every combination.
/// The total number of possible combinations (i.e. highest value possible) is 2^(# of Valves in the Circuit).
/// For example, if all open will be 0, no matter how many Valves are in the circuit
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
bool SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::IsCurrentValveStateUnique()
{
  uint64_t index = 1;
  uint64_t currentState = 0;
  /// The lower bits are used to represent the states of the non-polarized valves.  In the lexicographic
  /// ordering of all state representations, all non-polarized valve combinations are enumerated first before
  /// the polarized element combinations.  This is the order in which we want to evaluate the states
  /// so this bit order is important.
  for (PathType* pValve : m_circuit->GetValvePaths())
  {
    if (pValve->GetNextValve() == eGate::Closed)
      currentState |= index;
    index = index << static_cast<uint64_t>(1);
  }
  for (PathType* pPolarizedElement : m_circuit->GetPolarizedElementPaths())
  {
    if (pPolarizedElement->GetNextPolarizedState() == eGate::Closed)
      currentState |= index;
    index = index << static_cast<uint64_t>(1);
  }
  if (m_valveStates.find(currentState) != end(m_valveStates))
    return false;
  m_valveStates.insert(currentState);
  return true;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Add a value to the appropriate location in the A matrix.
///
/// \param  n                 The Node that is currently being analyzed.
/// \param  p                 The Path that is currently being analyzed.
/// \param  dMultiplier       The value to insert in the A matrix.
/// \param  hasPotentialSource True if the path has a Potential Source.
///
/// \details
/// Each row in the square A matrix is for the KCL analysis of a single Node.
/// Each column is for the variables (i.e. other Nodes) in that KCL equation, and is in the same order as the rows.
/// These should include all of the multipliers on the left side of the KCL equations.
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::PopulateAMatrix(NodeType& n, PathType& p, double dMultiplier, bool hasPotentialSource)
{
  NodeType& nSrc = p.GetSourceNode();
  NodeType& nTgt = p.GetTargetNode();

  if (std::isinf(dMultiplier))
  {
    //Someone screwed up
    //Probably a divide by zero and most likely from a Resistance
    m_ss << "Attempting to populate a matrix with an infinite value.  Check " << p.GetName() << " path circuit element.";
    Fatal(m_ss);
  }

  if (hasPotentialSource)
  {
    //Handle Potential Sources here.
    //The A Matrix uses their Flux as a variable.
    double sign = (&n == &nSrc) ? -1 : 1;
    _eigen->AMatrix(n.GetCalculatorIndex(), m_potentialSources[&p]) += sign;
  }
  else
  {
    //We use the convention that the Node we're sitting on is always the positive multiplier in the current equation.
    //Therefore, it's positive as either the Source or the Target.
    //Nodes on the other end of the path have a negative multiplier.

    if (nSrc.IsReferenceNode())
    {
      _eigen->bVector(n.GetCalculatorIndex()) += 0;
    }
    else if (&nSrc == &n)
    {
      //If the Source Node is the Node we're sitting on, our convention is positive.
      _eigen->AMatrix(n.GetCalculatorIndex(), nSrc.GetCalculatorIndex()) += dMultiplier;
    }
    else
    {
      //The Source Node is not the Node we're sitting on for KCL analysis.
      _eigen->AMatrix(n.GetCalculatorIndex(), nSrc.GetCalculatorIndex()) -= dMultiplier;
    }

    if (nTgt.IsReferenceNode())
    {
      _eigen->bVector(n.GetCalculatorIndex()) += 0;
    }
    else if (&nTgt == &n)
    {
      //If the Target Node is the Node we're sitting on, our convention is positive.
      _eigen->AMatrix(n.GetCalculatorIndex(), nTgt.GetCalculatorIndex()) += dMultiplier;
    }
    else
    {
      //The Target Node is not the Node we're sitting on for KCL analysis.
      _eigen->AMatrix(n.GetCalculatorIndex(), nTgt.GetCalculatorIndex()) -= dMultiplier;
    }
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Advances time
///
/// \param  Circuit The circuit in question.
/// \param  vSubstances List with the substances that are currently present in the circuit
///
/// \details
/// Move the Next circuit values to Current, and sets the Next values to the Baselines.
/// This effectively advances time for the circuit
//--------------------------------------------------------------------------------------------------
template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::PostProcess(CircuitType& circuit)
{
  // 1) Push Next potentials, fluxes, and modified path elements (the only elements that will have a value in Next are those that are modified since the last time step) to Current
  for (NodeType* n : circuit.GetNodes())
  {
    Override<PotentialUnit>(n->GetNextPotential(), n->GetPotential());
    //Quantity is only incremented, not overwritten by the circuit math.
    Override<QuantityUnit>(n->GetNextQuantity(), n->GetQuantity());
  }
  //The current values stay the same, unless there's a next value to modify it
  //This goes along with the circuit "alter" methodology
  for (PathType* p : circuit.GetPaths())
  {
    if (p->HasNextValve())
      p->SetValve(p->GetNextValve());
    if (p->HasNextSwitch())
      p->SetSwitch(p->GetNextSwitch());
    if (p->HasNextPolarizedState())
      p->SetPolarizedState(p->GetNextPolarizedState());

    Override<FluxUnit>(p->GetNextFlux(), p->GetFlux());
    Override<FluxUnit>(p->GetNextFluxSource(), p->GetFluxSource());
    Override<ResistanceUnit>(p->GetNextResistance(), p->GetResistance());
    Override<CapacitanceUnit>(p->GetNextCapacitance(), p->GetCapacitance());
    Override<InductanceUnit>(p->GetNextInductance(), p->GetInductance());
    Override<PotentialUnit>(p->GetNextPotentialSource(), p->GetPotentialSource());

    // 2) Set Next Path elements to the Baseline values
    //    We won't touch Valves or Switches.  Valves should keep their state to efficiently solve based on assumed states
    p->GetNextFluxSource().Set(p->GetFluxSourceBaseline());
    p->GetNextResistance().Set(p->GetResistanceBaseline());
    p->GetNextCapacitance().Set(p->GetCapacitanceBaseline());
    p->GetNextInductance().Set(p->GetInductanceBaseline());
    p->GetNextPotentialSource().Set(p->GetPotentialSourceBaseline());
  }
}

template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::ParseInPotentialSources()
{
  //Deal with potential sources
  //We also model closed Switches, "closed" Valves (those allowing flux), and shorts (paths without an element) as 0Pa Potential Sources.
  //All potential sources will have their Flux directly solved by adding equations for known Node potential differences caused by them.
  //We add rows for these equations after the KCL equations (hence the iNodeSize+i).
  for (auto itr : m_potentialSources)
  {
    PathType* p = itr.first;
    NodeType& nSrc = p->GetSourceNode();
    NodeType& nTgt = p->GetTargetNode();

    if (!nSrc.IsReferenceNode())
    {
      _eigen->AMatrix(itr.second, nSrc.GetCalculatorIndex()) = -1;
    }
    if (!nTgt.IsReferenceNode())
    {
      _eigen->AMatrix(itr.second, nTgt.GetCalculatorIndex()) = 1;
    }

    if (p->HasNextPotentialSource())
    {
      _eigen->bVector(itr.second) += p->GetNextPotentialSource().GetValue(m_PotentialUnit);
    }
    else
    {
      _eigen->bVector(itr.second) = 0.0;
    }
  }
}

template<CIRCUIT_CALCULATOR_TEMPLATE>
void SECircuitCalculator<CIRCUIT_CALCULATOR_TYPES>::Verbose(std::string location)
{
  std::cout << "Circuit Calculator Location: " << location << std::endl;
  std::cout << "Circuit: " << m_circuit->GetName() << std::endl;
    
  std::cout << "A = " << std::endl << _eigen->AMatrix << std::endl;
  std::cout << "b = " << std::endl << _eigen->bVector << std::endl;
  std::cout << "x = " << std::endl << _eigen->xVector << std::endl;

  std::cout << std::endl;

  std::ofstream  fout;
  fout.open("./test_results/unit_tests/pulse/PrePotential_aMatrix" + std::to_string(m_currentTime_s) + ".csv");
  fout << _eigen->AMatrix << std::endl;
  fout.close();
  fout.open("./test_results/unit_tests/pulse/PrePotential_bVector" + std::to_string(m_currentTime_s) + ".csv");
  fout << _eigen->bVector << std::endl;
  fout.close();
  fout.open("./test_results/unit_tests/pulse/PrePotential_xVector" + std::to_string(m_currentTime_s) + ".csv");
  fout << _eigen->xVector << std::endl;
  fout.close();
}

#include "cdm/blackbox/fluid/SEFluidBlackBox.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
template class SECircuitCalculator<SEFluidCircuit, SEFluidCircuitNode, SEFluidCircuitPath, VolumePerPressureUnit, VolumePerTimeUnit, PressureTimeSquaredPerVolumeUnit, PressureUnit, VolumeUnit, PressureTimePerVolumeUnit>;
#include "cdm/blackbox/electrical/SEElectricalBlackBox.h"
#include "cdm/circuit/electrical/SEElectricalCircuit.h"
template class SECircuitCalculator<SEElectricalCircuit, SEElectricalCircuitNode, SEElectricalCircuitPath, ElectricCapacitanceUnit, ElectricCurrentUnit, ElectricInductanceUnit, ElectricPotentialUnit, ElectricChargeUnit, ElectricResistanceUnit>;
#include "cdm/blackbox/thermal/SEThermalBlackBox.h"
#include "cdm/circuit/thermal/SEThermalCircuit.h"
template class SECircuitCalculator<SEThermalCircuit, SEThermalCircuitNode, SEThermalCircuitPath, HeatCapacitanceUnit, PowerUnit, HeatInductanceUnit, TemperatureUnit, EnergyUnit, HeatResistanceUnit>;
