# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(test C)

add_compile_options(-G)
add_executable(vas exe.c)
target_link_libraries(vas lib)
add_executable(dynamic)
target_compile_options(dynamic PRIVATE -dynamic)
add_dependencies(dynamic vas)
add_library(lib func.c)
set_target_properties( dynamic PROPERTIES ghs_integrity_app ON )
