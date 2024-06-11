cmake_minimum_required(VERSION 3.30)
cmake_policy(SET CMP0171 NEW)

# The incremental linking flags do not change output in a manner we can
# introspec, so override our table of flags to specify additional flags
# we can use to verify the behavior.
set(CMAKE_USER_MAKE_RULES_OVERRIDE_C ${CMAKE_CURRENT_SOURCE_DIR}/override-C.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/override-CXX.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CUDA ${CMAKE_CURRENT_SOURCE_DIR}/override-CUDA.cmake)

project(MSVCIncrementalLink)

if(CMake_TEST_CUDA)
  enable_language(CUDA)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

function(verify lang src)
  # TODO: define symbol names here to check in post-build commands
  # Define symbols in verify.h
  # Add post-build commands to check exported symbols
  set(verify_def_Default -DVERIFY_DEFAULT)
  set(verify_def_Force -DVERIFY_FORCE)
  set(verify_def_Never -DVERIFY_NEVER)
  foreach(inc Default Force Never)
      # Test that try_compile builds with this incremental linking setting.
      set(CMAKE_MSVC_INCREMENTAL_LINK "${inc}")
      try_compile(${inc}_COMPILES
        ${CMAKE_CURRENT_BINARY_DIR}/try_compile/${inc}
        ${CMAKE_CURRENT_SOURCE_DIR}/${src}
        COMPILE_DEFINITIONS ${verify_def_${inc}}
        OUTPUT_VARIABLE ${inc}_OUTPUT
        )
      if(${inc}_COMPILES)
        message(STATUS "try_compile ${lang} with ${inc} worked")
      else()
        string(REPLACE "\n" "\n  " ${inc}_OUTPUT "  ${${inc}_OUTPUT}")
        message(SEND_ERROR "try_compile ${lang} with ${inc} failed:\n${${inc}_OUTPUT}")
      endif()

      # Test that targets link with the incremental linking setting.
      set(CMAKE_MSVC_INCREMENTAL_LINK "$<$<BOOL:$<TARGET_PROPERTY:BOOL_TRUE>>:${inc}>$<$<BOOL:$<TARGET_PROPERTY:BOOL_FALSE>>:BadContent>")
      add_library(${inc}-${lang} SHARED ${src})
      set_property(TARGET ${inc}-${lang} PROPERTY BOOL_TRUE TRUE)
      target_compile_definitions(${inc}-${lang} PRIVATE ${verify_def_${inc}})
  endforeach()
endfunction()

verify(C verify.c)
verify(CXX verify.cxx)
if(CMake_TEST_CUDA)
  verify(CUDA verify.cu)
endif()
