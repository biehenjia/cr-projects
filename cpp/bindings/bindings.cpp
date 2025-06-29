#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "_ast.hpp"
#include "crobj.hpp"
#include "crnum.hpp"
#include "crsum.hpp"
#include "crprod.hpp"
#include "crexpr.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pycr, m) {
    m.doc() = "pybind11 bindings for crexpr and AST evaluation";

    // Binary operator types
    py::enum_<bt>(m, "bt")
        .value("ADD", bt::ADD)
        .value("MUL", bt::MUL)
        .value("DIV", bt::DIV)
        .value("SUB", bt::SUB)
        .value("POW", bt::POW)
        .export_values();

    // Unary operator types
    py::enum_<ut>(m, "ut")
        .value("COS", ut::COS)
        .value("SIN", ut::SIN)
        .value("EXP", ut::EXP)
        .value("LN", ut::LN)
        .export_values();

    // Base AST node
    py::class_<ASTnode>(m, "ASTnode")
        .def("crinit", &ASTnode::crinit, "Initialize the CR object from this AST", py::arg("x"), py::arg("h"))
        .def("creval", &ASTnode::creval, "Evaluate the initialized CR object for q steps", py::arg("q"));

    // Numeric literal
    py::class_<ASTnum, ASTnode>(m, "ASTnum")
        .def(py::init<double>(), "Create a numeric AST node", py::arg("value"));

    // Variable (x,h)
    py::class_<ASTvar, ASTnode>(m, "ASTvar")
        .def(py::init<>(), "Create a variable AST node");

    // Binary operation
    py::class_<ASTbin, ASTnode>(m, "ASTbin")
        .def(py::init<bt, ASTnode*, ASTnode*>() ,"Create a binary AST node", py::arg("op"), py::arg("left"), py::arg("right"));

    // Unary operation
    py::class_<ASTun, ASTnode>(m, "ASTun")
        .def(py::init<ut, ASTnode*>(),"Create a unary AST node", py::arg("op"), py::arg("child"));
}