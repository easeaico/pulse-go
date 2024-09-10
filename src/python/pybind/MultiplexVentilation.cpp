/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include "MVEngine.h"

namespace py = pybind11;
using namespace pulse::study::multiplex_ventilation;

void MultiplexVentilationEngineBind(py::module& m)
{
  py::class_<MVEngine>(m, "MVEngine", py::buffer_protocol())
    .def(py::init<std::string const&, bool, std::string const&>())
    .def("create_engine", (bool (MVEngine::*)(const std::string & simulationDataStr)) &MVEngine::CreateEngine)
    .def("advance_time", &MVEngine::AdvanceTime)
    .def("process_actions", &MVEngine::ProcessActions)
    .def("get_patient_state", (std::string (MVEngine::*)()) &MVEngine::GetSimulationState)
    .def("destroy_engine", &MVEngine::DestroyEngines)
    ;
}