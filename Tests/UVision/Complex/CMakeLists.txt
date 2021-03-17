cmake_minimum_required(VERSION 3.20)
include(../default.cmake)

project(Complex C ASM)

add_subdirectory(Library)

add_executable(Executable main.c)
target_link_libraries(Executable Library)

add_library(ARM::CMSIS:CORE MODULE IMPORTED)
set_target_properties(ARM::CMSIS:CORE PROPERTIES
        IMPORTED_LOCATION     "Pack"
        UV_PACK               "ARM.CMSIS"
        UV_COMPONENT          "ARM::CMSIS:CORE")

add_library(ARM::Device:Startup MODULE IMPORTED)
set_target_properties(ARM::Device:Startup PROPERTIES
        IMPORTED_LOCATION     "Pack"
        UV_PACK               "ARM.CMSIS"
        UV_COMPONENT          "ARM::Device:Startup")

target_link_libraries(Executable ARM::CMSIS:CORE ARM::Device:Startup)
