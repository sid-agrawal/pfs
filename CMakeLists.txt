cmake_minimum_required (VERSION 3.5)

project (pfs)

# ------------------------------------------------------------------------
# Flags that are expected to be shared when added as sub-directory
# ------------------------------------------------------------------------

option (pfs_BUILD_SHARED_LIBS "Build a shared library instead of static one" ${BUILD_SHARED_LIBS})
if (pfs_BUILD_SHARED_LIBS)
    set (pfs_SHARED_OR_STATIC "SHARED")
else ()
    set (pfs_SHARED_OR_STATIC "STATIC")
endif ()

if (NOT CMAKE_BUILD_TYPE)
    message (STATUS "Build type not specified. Set by default to DEBUG!")
    set (CMAKE_BUILD_TYPE Debug)
endif ()

# ------------------------------------------------------------------------
# Flags that are NOT expected to be shared when added as sub-directory
# ------------------------------------------------------------------------

option (pfs_BUILD_COVERAGE "Enable coverage instrumentation" OFF)
option (pfs_BUILD_ASAN "Enable address sanitizer" OFF)
option (pfs_BUILD_SAMPLES "Build samples" ON)
option (pfs_BUILD_TESTS "Build tests" ON)

# ------------------------------------------------------------------------
# Actual configuration
# ------------------------------------------------------------------------

add_compile_options (-Wall -Wextra -pedantic -Werror)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set (pfs_ROOT_SOURCE_DIR src)
aux_source_directory (${pfs_ROOT_SOURCE_DIR} pfs_ROOT_SOURCES)
aux_source_directory (${pfs_ROOT_SOURCE_DIR}/parsers pfs_PARSERS_SOURCES)
set (SOURCES ${pfs_ROOT_SOURCES} ${pfs_PARSERS_SOURCES})

add_library (pfs ${pfs_SHARED_OR_STATIC} ${SOURCES})
target_compile_features(pfs PUBLIC cxx_std_11)
target_include_directories(
    pfs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (pfs_BUILD_COVERAGE)
    set (pfs_BUILD_COVERAGE_FLAGS -O0 --coverage)
    target_compile_options (pfs PUBLIC ${pfs_BUILD_COVERAGE_FLAGS})
    target_link_libraries (pfs PUBLIC ${pfs_BUILD_COVERAGE_FLAGS})
endif ()

if (pfs_BUILD_ASAN)
    set (pfs_BUILD_ASAN_FLAGS -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak)
    target_compile_options (pfs PUBLIC ${pfs_BUILD_ASAN_FLAGS})
    target_link_libraries (pfs PUBLIC -lasan ${pfs_BUILD_ASAN_FLAGS})
endif ()

if (pfs_BUILD_SAMPLES)
    aux_source_directory (sample pfs_SAMPLE_SOURCES)
    add_executable (sample ${pfs_SAMPLE_SOURCES})
    target_compile_features(sample PUBLIC cxx_std_11)
    target_link_libraries (sample PRIVATE pfs)
endif()

if (pfs_BUILD_TESTS)
    set (pfs_UNITTEST_SOURCE_DIR test)
    aux_source_directory (${pfs_UNITTEST_SOURCE_DIR} pfs_UNITTEST_SOURCES)
    add_executable (unittest ${pfs_UNITTEST_SOURCES})
    target_compile_features(unittest PUBLIC cxx_std_11)
    target_link_libraries (unittest PRIVATE pfs)
endif()

if (pfs_BUILD_TESTS AND pfs_BUILD_COVERAGE)
    set (pfs_BUILD_TESTS_COVERAGE_FLAGS --coverage)
    target_compile_options (pfs PUBLIC ${pfs_BUILD_TESTS_COVERAGE_FLAGS})
    target_link_libraries (pfs PUBLIC ${pfs_BUILD_TESTS_COVERAGE_FLAGS})
endif ()

# ------------------------------------------------------------------------
# Generate a python module
# ------------------------------------------------------------------------

set(PYBIND11_PYTHON_VERSION 3.6)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC")

option (pfs_BUILDROOT "Build for a builtroot environment" OFF)

if (pfs_BUILDROOT)
    set(pybind11_DIR /lib/python3.10/site-packages/pybind11/share/cmake/pybind11)
else ()
    set(pybind11_DIR ${CMAKE_SOURCE_DIR}/../venv/lib/python3.10/site-packages/pybind11/share/cmake/pybind11)
endif()

find_package(pybind11 REQUIRED)

aux_source_directory (${pfs_ROOT_SOURCE_DIR}/pybind pfs_PYBIND_SOURCES)
pybind11_add_module(pypfs ${pfs_PYBIND_SOURCES})

target_compile_definitions(pypfs PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_include_directories(pypfs PRIVATE ${PYBIND11_INCLUDE_DIRS})
target_link_libraries(pypfs PRIVATE pfs)

# ------------------------------------------------------------------------
# Generate a CMake package configuration file to support find_package()
# ------------------------------------------------------------------------

include(CMakePackageConfigHelpers)
include(GNUInstallDirs) # Sets a default value for CMAKE_INSTALL_LIBDIR

set (pfs_PACKAGE_CONFIG ${PROJECT_NAME}-config.cmake)
set (pfs_PACKAGE_CONFIG_IN ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${pfs_PACKAGE_CONFIG}.in)
set (pfs_PACKAGE_CONFIG_OUT ${CMAKE_CURRENT_BINARY_DIR}/${pfs_PACKAGE_CONFIG})

set (pfs_PACKAGE_INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

set (pfs_EXPORT_NAME ${PROJECT_NAME}Targets)

configure_package_config_file(
    ${pfs_PACKAGE_CONFIG_IN}
    ${pfs_PACKAGE_CONFIG_OUT}
    INSTALL_DESTINATION ${pfs_PACKAGE_INSTALL_DESTINATION}
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${pfs_EXPORT_NAME}
)

install(
    EXPORT ${pfs_EXPORT_NAME}
    FILE ${pfs_PACKAGE_CONFIG}
    DESTINATION ${pfs_PACKAGE_INSTALL_DESTINATION}
)

install(
    DIRECTORY include/
    DESTINATION include
)
