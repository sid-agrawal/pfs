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

    py::class_<pfs::mount>(m, "mount")
    .def_readwrite("id", &pfs::mount::id)
    .def_readwrite("parent_id", &pfs::mount::parent_id)
    .def_readwrite("device", &pfs::mount::device)
    .def_readwrite("root", &pfs::mount::root)
    .def_readwrite("point", &pfs::mount::point)
    .def_readwrite("options", &pfs::mount::options)
    .def_readwrite("optional", &pfs::mount::optional)
    .def_readwrite("filesystem_type", &pfs::mount::filesystem_type)
    .def_readwrite("source", &pfs::mount::source)
    .def_readwrite("super_options", &pfs::mount::super_options)
    ;

    py::class_<pfs::task_status::uid_set>(m, "uid_set")
    .def_readwrite("effective", &pfs::task_status::uid_set::effective)
    ;

    py::class_<pfs::capabilities_mask>(m, "capabilities_mask")
    .def_readwrite("raw", &pfs::capabilities_mask::raw)
    ;

    // This class has a lot more fields, if we need them then we can add more def_readwrite
    py::class_<pfs::task_status>(m, "task_status")
    .def_readwrite("ns_pid", &pfs::task_status::ns_pid)
    .def_readwrite("uid", &pfs::task_status::uid)
    .def_readwrite("gid", &pfs::task_status::gid)
    .def_readwrite("cap_inh", &pfs::task_status::cap_inh)
    .def_readwrite("cap_prm", &pfs::task_status::cap_prm)
    .def_readwrite("cap_eff", &pfs::task_status::cap_eff)
    .def_readwrite("cap_bnd", &pfs::task_status::cap_bnd)
    .def_readwrite("cap_amb", &pfs::task_status::cap_amb)
    ;

    py::class_<pfs::task>(m, "task")
    .def("get_maps", &pfs::task::get_maps)
    .def("get_status", &pfs::task::get_status)
    .def("get_mountinfo", &pfs::task::get_mountinfo)
    .def("get_cgroups", &pfs::task::get_cgroups)
    // We need a special cast for get_ns because it is an overloaded function
    .def("get_ns", static_cast<std::unordered_map<std::string, ino_t> (pfs::task::*)(void) const>(&pfs::task::get_ns))
    ;
    
    py::class_<pfs::cgroup_controller>(m, "cgroup_controller")
    .def_readwrite("subsys_name", &pfs::cgroup_controller::subsys_name)
    .def_readwrite("hierarchy", &pfs::cgroup_controller::hierarchy)
    .def_readwrite("num_cgroups", &pfs::cgroup_controller::num_cgroups)
    .def_readwrite("enabled", &pfs::cgroup_controller::enabled)
    ;
    py::class_<pfs::cgroup>(m, "cgroup")
    .def_readwrite("hierarchy", &pfs::cgroup::hierarchy)
    .def_readwrite("controllers", &pfs::cgroup::controllers)
    .def_readwrite("pathname", &pfs::cgroup::pathname)
    ;

    py::class_<pfs::procfs>(m, "procfs")
    .def(py::init<>())  
    .def("get_processes", &pfs::procfs::get_processes)
    .def("get_task", &pfs::procfs::get_task, py::arg("id"))
    .def("get_cgroups", &pfs::procfs::get_cgroups)
    ;
}