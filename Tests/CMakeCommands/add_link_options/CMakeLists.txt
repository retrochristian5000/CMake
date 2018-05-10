cmake_minimum_required(VERSION 3.11)

project(add_link_options LANGUAGES C)


set(obj "${CMAKE_C_OUTPUT_EXTENSION}")
if(BORLAND)
  set(pre -)
endif()

add_link_options(${pre}BADFLAG${obj})

add_executable(add_link_options LinkOptionsExe.c)

get_target_property(result add_link_options LINK_OPTIONS)
if (NOT result MATCHES "${pre}BADFLAG${obj}")
  message(SEND_ERROR "add_link_options not populated the LINK_OPTIONS target property")
endif()


add_library(imp UNKNOWN IMPORTED)
get_target_property(result imp LINK_OPTIONS)
if (result)
  message(FATAL_ERROR "add_link_options populated the LINK_OPTIONS target property")
endif()
