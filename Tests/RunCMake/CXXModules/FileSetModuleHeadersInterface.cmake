add_library(module-header)
target_sources(module-header
  INTERFACE
    FILE_SET fs TYPE CXX_MODULE_HEADERS FILES
      sources/module-header.h)
