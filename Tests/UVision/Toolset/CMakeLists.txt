cmake_minimum_required(VERSION 3.18)
include(../default.cmake)

# Change toolset to ArmCC
set(CMAKE_GENERATOR_TOOLSET "ArmCC")

set(TARGET Toolset)
project(${TARGET} C)

add_library(${TARGET} STATIC foo.c)
