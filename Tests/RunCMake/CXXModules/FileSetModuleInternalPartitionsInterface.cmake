enable_language(CXX)

add_library(module-internal-partition)
target_sources(module-internal-partition
  INTERFACE
    FILE_SET fs TYPE CXX_MODULE_INTERNAL_PARTITIONS FILES
      sources/module-internal-part.cxx)
