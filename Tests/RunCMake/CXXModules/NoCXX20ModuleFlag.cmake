enable_language(CXX)

if (NOT CMAKE_CXX_STANDARD_DEFAULT)
  set(CMAKE_CXX_STANDARD_DEFAULT "11")
endif ()

add_library(noexperimentalflag)
target_sources(noexperimentalflag
  PUBLIC
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
target_compile_features(noexperimentalflag
  PRIVATE
    cxx_std_20)
