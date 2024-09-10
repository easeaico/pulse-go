/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <pybind11/pybind11.h>
#include "PulseEngineThunk.h"

namespace py = pybind11;

// Core Physiology Engine
void PulseEngineBind(py::module&);
void PulseEnginePoolBind(py::module&);
// Studies
#ifndef EXCLUDE_STUDIES
void MultiplexVentilationEngineBind(py::module&);
#endif

double convert(double val, const std::string& from, const std::string& to)
{
  CPScalar cp(val, from);
  double d = cp(to).GetValue();
  return d;
}

bool execute_scenario(const std::string& exec_str, eSerializationFormat fmt)
{
  return PulseEngineThunk::ExecuteScenario(exec_str, fmt, nullptr);
}

PYBIND11_MODULE(PyPulse, m)
{
   m.doc() = R"pbdoc(
        Pybind11 Pulse Physiology Engine
        -----------------------
        .. currentmodule:: pulse
        .. autosummary::
           :toctree: _generate
           serialize_from_file
           serialize_to_file
           serialize_from_string
           serialize_to_string
    )pbdoc";

   m.def("convert", &convert, "A function that converts a number from one unit to another");

   py::enum_<eSerializationFormat>(m, "serialization_format")
    .value("binary", eSerializationFormat::BINARY)
    .value("json", eSerializationFormat::JSON)
    .export_values();
   py::enum_<eModelType>(m, "model_type")
     .value("human_adult_whole_body", eModelType::HumanAdultWholeBody)
     .value("human_adult_ventilation_mechanics", eModelType::HumanAdultVentilationMechanics)
     .export_values();

  PulseEngineBind(m);
  PulseEnginePoolBind(m);
#ifndef EXCLUDE_STUDIES
  MultiplexVentilationEngineBind(m);
#endif

  m.def("execute_scenario", &execute_scenario, "Executes a scenario file");

  m.attr("__version__") = PulseBuildInformation::Version();
  m.attr("__hash__") = PulseBuildInformation::Hash();
}
