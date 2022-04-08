enable_language(CXX)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
# Fake C++20 support to provoke the "generator does not support C++ modules"
# error message..
set(CMAKE_CXX_COMPILE_FEATURES cxx_std_20)
set(CMAKE_CXX_STANDARD_DEFAULT 11)

add_library(nodyndep)
target_sources(nodyndep
  PUBLIC
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
target_compile_features(nodyndep
  PRIVATE
    cxx_std_20)
