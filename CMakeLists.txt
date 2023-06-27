cmake_minimum_required(VERSION 3.12.0)

project(xdd VERSION 2.0 LANGUAGES C)

if (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  message(FATAL_ERROR "Unsupported Build Platform ${CMAKE_SYSTEM_NAME}")
endif()

# package properties
set(PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(PACKAGE_STRING "${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
set(PACKAGE_URL "")
set(PACKAGE_BUGREPORT "bug@xdd.org")
set(PACKAGE_TARNAME "${CMAKE_PROJECT_NAME}")

# Python is required
find_program(PYTHON REQUIRED NAMES python3 python)

# Threads are required
find_package(Threads REQUIRED)

# for finding packages with pkg-config
find_package(PkgConfig REQUIRED)

include(CheckIncludeFiles)
include(CheckSymbolExists)

# find libibverbs
find_library(IBVERBS ibverbs)
if (IBVERBS)
  message(STATUS "libibverbs: ${IBVERBS}")
  check_include_files(infiniband/verbs.h HAVE_INFINIBAND_VERBS_H)

  list(APPEND CMAKE_REQUIRED_LIBRARIES -libverbs) # CMP0075, requires CMake 3.12
  check_symbol_exists(ibv_get_device_list "infiniband/verbs.h" IBV_GET_DEVICE_LIST)
  list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES -libverbs)
  if (IBV_GET_DEVICE_LIST)
    option(WITH_VERBS "Build with verbs" ON)
    if (WITH_VERBS)
      set(HAVE_ENABLE_IB 1)
    endif()
  endif()
else()
  message(STATUS "libibverbs not found")
endif()

# libnuma is optional
pkg_search_module(NUMA numa)
if (NUMA_FOUND)
  message(STATUS "libnuma CFLAGS:  ${NUMA_CFLAGS}")
  message(STATUS "libnuma LDFLAGS: ${NUMA_LDFLAGS}")

  option(WITH_NUMA "Build with libnuma" ON)
  if (WITH_NUMA)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${NUMA_CFLAGS}")

    list(APPEND CMAKE_REQUIRED_LIBRARIES ${NUMA_LDFLAGS}) # CMP0075, requires CMake 3.12
    check_symbol_exists(numa_node_to_cpus     "numa.h" HAVE_NUMA_NODE_TO_CPUS)
    check_symbol_exists(numa_allocate_cpumask "numa.h" HAVE_NUMA_ALLOCATE_CPUMASK)
    list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES ${NUMA_LDFLAGS})

    set(HAVE_NUMA_H 1)
  endif()
else()
  message(STATUS "libnuma not found")
endif()

# optionally build for flamegraph analysis
option(WITH_FLAMEGRAPH "Build for flamegraph analysis" ON)
if (WITH_FLAMEGRAPH)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer")
endif()

# build library and executables
add_subdirectory(src)

# set up tests
enable_testing()
add_subdirectory(tests)

# optionally build docs
find_package(Doxygen COMPONENTS dot)
if (DOXYGEN_FOUND)
  option(WITH_DOXYGEN "Build doxygen documentation" ON)
  if (WITH_DOXYGEN)
   set(DOXYGEN_GENERATE_HTML  ON)
   set(DOXYGEN_GENERATE_MAN   ON)
   set(DOXYGEN_GENERATE_LATEX ON)
   set(DOXYGEN_PDF_HYPERLINKS ON)

    doxygen_add_docs(doxygen
      "${CMAKE_SOURCE_DIR}/doc"
      )
  endif()
endif()

# build RPM
find_program(RPMBUILD rpmbuild)
if (RPMBUILD)
  message(STATUS "rpmbuild: ${RPMBUILD}")

  option(RPM "Build RPM" ON)
  if (RPM)
    set(CPACK_GENERATOR "RPM")
    set(CPACK_RPM_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
    set(CPACK_RPM_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
    set(CPACK_RPM_PACKAGE_SUMMARY "IO performace Test Tool dd with an X factor")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL-2.0")
    set(CPACK_RPM_PACKAGE_GROUP "Benchmarking")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/MigeljanImeri/xdd")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "IO Test tool that can do so much more.")
  endif()
endif()

# build DEB
find_program(DPKGBUILD dpkg-buildpackage)
if (DPKGBUILD)
  message(STATUS "dpkg-buildpackage: ${DPKGBUILD}")

  option(DPKG "Build DEB" ON)
  if (DPKG)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
    set(CPACK_DEBIAN_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
    set(CPACK_DEBIAN_PACKAGE_SUMMARY "IO performace Test Tool dd with an X factor")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/MigeljanImeri/xdd")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "XDD Developers")
  endif()
endif()

if ((RPMBUILD AND RPM) OR
    (DPKGBUILD AND DPKG))
    include(CPack)
endif()
