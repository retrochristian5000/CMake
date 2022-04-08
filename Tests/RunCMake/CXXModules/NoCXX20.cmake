enable_language(CXX)
# Fake C++17 support to provoke the "generator does not support C++ modules"
# error message..
set(CMAKE_CXX_COMPILE_FEATURES cxx_std_17)
set(CMAKE_CXX_STANDARD_DEFAULT 11)

add_library(nocxx20)
target_sources(nocxx20
  PUBLIC
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
target_compile_features(nocxx20
  PRIVATE
    cxx_std_17)
