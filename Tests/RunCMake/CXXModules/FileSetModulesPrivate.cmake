enable_language(CXX)

add_library(module)
target_sources(module
  PRIVATE
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
