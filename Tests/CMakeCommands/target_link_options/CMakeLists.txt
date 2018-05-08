
cmake_minimum_required(VERSION 3.11)

project(target_link_options LANGUAGES C)

set(obj "${CMAKE_C_OUTPUT_EXTENSION}")
if(BORLAND)
  set(pre -)
endif()

add_library(target_link_options_lib SHARED LinkOptionsLib.c)
target_link_options(target_link_options_lib
  PRIVATE ${pre}BADFLAG_PRIVATE${obj}
  PUBLIC ${pre}BADFLAG_PUBLIC${obj}
  INTERFACE ${pre}BADFLAG_INTERFACE${obj})


add_library(target_link_options_producer SHARED LinkOptionsLib.c)
target_link_options(target_link_options_producer
  INTERFACE ${pre}BADFLAG_INTERFACE${obj})

add_executable(target_link_options_consumer LinkOptionsExe.c)
target_link_libraries(target_link_options_consumer PRIVATE target_link_options_producer)


add_library(target_link_options SHARED LinkOptionsLib.c)
# Test no items
target_link_options(target_link_options PRIVATE)
