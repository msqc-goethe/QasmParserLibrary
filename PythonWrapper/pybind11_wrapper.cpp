#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <parser.h>

namespace py = pybind11;

PYBIND11_MODULE(openQasmParser, m) {
//  m.doc() = "Python binding for the OpenQASM parser library.";
//  m.def("parseCircuit", &qasmparser::parseCircuit, "Parse an ansatz into the corresponding OpenQASM representation",
//        py::arg("input_fn"), py::arg("use_omp"), py::arg("version") = 2, py::arg("out_fn"), py::arg("multiplier"));

    m.def("test", &qasmparser::test, "test function", py::arg("a"), py::arg("b"));
}
