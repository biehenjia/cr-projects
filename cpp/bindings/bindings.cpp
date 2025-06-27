#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <_ast.hpp>
#include <memory>
#include <string>
#include <vector>


namespace py = pybind11; 

PYBIND11_MODULE(crapi, m) {
    m.doc() = "cpp bindings"
    py::enum
}