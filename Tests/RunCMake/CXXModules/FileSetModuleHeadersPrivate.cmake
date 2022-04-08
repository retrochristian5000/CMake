enable_language(CXX)

add_library(module-header
  sources/cxx-anchor.cxx)
target_sources(module-header
  PRIVATE
    FILE_SET fs TYPE CXX_MODULE_HEADER_UNITS FILES
      sources/module-header.h)
