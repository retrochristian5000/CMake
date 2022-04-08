enable_language(CXX)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
# Fake C++20 support to provoke the "generator does not support C++ modules"
# error message..
set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-dummy-flag=c++20")
set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-dummy-flag=c++20-ext")

add_library(nodyndep)
target_sources(nodyndep
  PUBLIC
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
target_compile_features(nodyndep
  PRIVATE
    cxx_std_20)
