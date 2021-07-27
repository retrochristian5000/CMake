cmake_minimum_required(VERSION 3.21)
project(FortranExternal NONE)

include(ExternalProject)
set(install_dir "${CMAKE_CURRENT_BINARY_DIR}/Produce-install")
ExternalProject_Add(produce
  SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Produce"
  BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}/Produce-build"
  PREFIX      "${CMAKE_CURRENT_BINARY_DIR}/Produce-build/root"
  INSTALL_DIR "${install_dir}"
  CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
  )

ExternalProject_Add(consume
  DEPENDS produce
  SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Consume"
  BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}/Consume-build"
  PREFIX      "${CMAKE_CURRENT_BINARY_DIR}/Consume-build/root"
  INSTALL_COMMAND ""
  CMAKE_ARGS "-DFortranStatic_ROOT=${install_dir}"
  )
