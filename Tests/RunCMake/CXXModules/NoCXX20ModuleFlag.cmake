enable_language(CXX)
# Fake C++20 support to provoke the "generator does not support C++ modules"
# error message..
set(CMAKE_CXX_COMPILE_FEATURES cxx_std_20)
set(CMAKE_CXX_STANDARD_DEFAULT "")

add_library(noexperimentalflag)
target_sources(noexperimentalflag
  PUBLIC
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
target_compile_features(noexperimentalflag
  PRIVATE
    cxx_std_20)
