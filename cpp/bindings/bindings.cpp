#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "_ast.hpp"
#include "crobj.hpp"
#include "crnum.hpp"
#include "crsum.hpp"
#include "crprod.hpp"
#include "crexpr.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pycrlib, m) {
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
        .value("TAN",ut::TAN)
        .export_values();

    // Base AST node
    py::class_<ASTnode>(m, "ASTnode")
        .def("crinit", &ASTnode::crinit, "Initialize the CR object from this AST", py::arg("x"), py::arg("h"))
        //.def("creval", &ASTnode::creval, "Evaluate the initialized CR object for q steps", py::arg("q"))
        .def("creval", [](ASTnode &self, size_t q) {
            py::array_t<double> result(q);
            {
                //std::cout<<"Creval called\n";
                double *ptr = static_cast<double*>(result.request().ptr);
                for (size_t i = 0; i < q; ++i) {
                    ptr[i] = self.cr->valueof();
                    self.cr->shift();
                }
                //std::cout<<"Creval Finished\n";
            }
            return result;
        }, 
        py::arg("q"))
        .def("view",&ASTnode::view, "Print tree information for AST node");

    // Numeric literal
    py::class_<ASTnum, ASTnode>(m, "ASTnum")
        .def(py::init<double>(), "Create a numeric AST node", py::arg("value"));

    // Variable (x,h)
    py::class_<ASTvar, ASTnode>(m, "ASTvar")
        .def(py::init<>(), "Create a variable AST node");

    // Binary operation
    py::class_<ASTbin, ASTnode>(m, "ASTbin", py::dynamic_attr() )
        .def(py::init<bt, ASTnode*, ASTnode*>() ,"Create a binary AST node", py::arg("op"), py::arg("left"), py::arg("right"),
        py::keep_alive<1, 2>(),
        py::keep_alive<1, 3>()
    );

    // Unary operation
    py::class_<ASTun, ASTnode>(m, "ASTun",py::dynamic_attr() )
        .def(py::init<ut, ASTnode*>(),"Create a unary AST node", py::arg("op"), py::arg("child"),
        py::keep_alive<1, 2>()
    );
}