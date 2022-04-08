enable_language(CXX)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

add_library(nodyndep)
target_sources(nodyndep
  PUBLIC
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
target_compile_features(nodyndep
  PRIVATE
    cxx_std_20)
