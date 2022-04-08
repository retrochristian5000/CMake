enable_language(CXX)

add_library(module)
target_sources(module
  INTERFACE
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
