#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <parser.h>

namespace py = pybind11;

PYBIND11_MODULE(openqasmparser, m) {
  m.doc() = "Python binding for the OpenQASM parser library.";
  m.def("parse_circuit", &qasmparser::parseCircuit, "Parse an ansatz into the corresponding OpenQASM representation, "
                                                    "parallel execution enabled.\n"
                                                    "@param input_fn: Path to the input file to parse.\n"
                                                    "@param version: OpenQASM version to use, default 3.\n"
                                                    "@param use_omp: Use OpenMP parallelism, default execution policy "
                                                    "parallelism.\n"
                                                    "@param parameterize: Set true to parameterize the circuit.\n"
                                                    "@param output_fn: Path to (non-)existing file to store the OpenQASM"
                                                    "representation file. (optional)\n"
                                                    "@param multiplier: Floating value to be multiplied to each "
                                                    "operator. (optional)",
        py::arg("input_fn"),  // Input file name
        py::kw_only(),
        py::arg("version") = 3,  // OpenQASM version (default v3); Parameterization requires v3!
        py::arg("use_omp") = false,   // Specify to use OpenMP parallelism
        py::arg("parameterize") = true,  // Indicate parameterized ansatz
        py::arg_v("output_fn", std::nullopt, "None"),  // Optional output file name
        py::arg_v("multiplier", std::nullopt, "None"));  // Optional multiplier
}
