cmake_minimum_required(VERSION 3.11)
project(LinkOptions LANGUAGES C)

set(obj "${CMAKE_C_OUTPUT_EXTENSION}")
if(BORLAND)
  set(pre -)
endif()

add_library(LinkOptions_dll SHARED LinkOptionsLib.c)
set_property(TARGET LinkOptions_dll PROPERTY LINK_OPTIONS ${pre}BADFLAG${obj}
  "SHELL:" # produces no options
  )

add_library(LinkOptions_mod MODULE LinkOptionsLib.c)
set_property(TARGET LinkOptions_mod PROPERTY LINK_OPTIONS "SHELL:${pre}BADFLAG${obj}")

add_executable(LinkOptions_exe LinkOptionsExe.c)
set_property(TARGET LinkOptions_exe PROPERTY LINK_OPTIONS ${pre}BADFLAG${obj})

add_library(LinkOptions_dll_config SHARED LinkOptionsLib.c)
set_property(TARGET LinkOptions_dll_config PROPERTY LINK_OPTIONS $<$<CONFIG:${TEST_CONFIG}>:${pre}BADFLAG_$<UPPER_CASE:${TEST_CONFIG}>${obj}>)

add_library(LinkOptions_mod_config MODULE LinkOptionsLib.c)
set_property(TARGET LinkOptions_mod_config PROPERTY LINK_OPTIONS $<$<CONFIG:${TEST_CONFIG}>:${pre}BADFLAG_$<UPPER_CASE:${TEST_CONFIG}>${obj}>)

add_executable(LinkOptions_exe_config LinkOptionsExe.c)
set_property(TARGET LinkOptions_exe_config PROPERTY LINK_OPTIONS $<$<CONFIG:${TEST_CONFIG}>:${pre}BADFLAG_$<UPPER_CASE:${TEST_CONFIG}>${obj}>)

add_executable(LinkOptions LinkOptionsExe.c)

add_subdirectory(LinkerOptions)
add_subdirectory(LinkerOptionsConfig)
