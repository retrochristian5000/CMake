include(Compiler/FujitsuClang)
include(Compiler/Clang-C)

if(NOT DEFINED CMAKE_C_COMPILE_OPTIONS_EXPLICIT_LANGUAGE)
  set(CMAKE_C_COMPILE_OPTIONS_EXPLICIT_LANGUAGE -x c)
endif()
