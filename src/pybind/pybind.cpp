/**
 * Exposes the procfs module for python using pybind11
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pfs/procfs.hpp>

namespace py = pybind11;
constexpr auto byref = py::return_value_policy::reference_internal;

PYBIND11_MODULE(pypfs, m) {
    m.doc() = "extract data from procfs";

    py::class_<pfs::mem_perm>(m, "mem_perm")
    .def_readwrite("can_read", &pfs::mem_perm::can_read)
    .def_readwrite("can_write", &pfs::mem_perm::can_write)
    .def_readwrite("can_execute", &pfs::mem_perm::can_execute)
    .def_readwrite("is_shared", &pfs::mem_perm::is_shared)
    .def_readwrite("is_private", &pfs::mem_perm::is_private)
    ;

    py::class_<pfs::mem_region>(m, "mem_region")
    .def_readwrite("start_address", &pfs::mem_region::start_address)
    .def_readwrite("end_address", &pfs::mem_region::end_address)
    .def_readwrite("perm", &pfs::mem_region::perm)
    .def_readwrite("device", &pfs::mem_region::device)
    .def_readwrite("pathname", &pfs::mem_region::pathname)
    ;

    py::class_<pfs::task>(m, "task")
    .def("get_maps", &pfs::task::get_maps)
    ;

    py::class_<pfs::procfs>(m, "procfs")
    .def(py::init<>())  
    .def("get_processes", &pfs::procfs::get_processes)
    .def("get_task", &pfs::procfs::get_task, py::arg("id"))
    ;
}