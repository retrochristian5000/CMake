enable_language(CXX)

add_library(module-internal-partition)
target_sources(module-internal-partition
  PRIVATE
    FILE_SET fs TYPE CXX_MODULE_INTERNAL_PARTITIONS FILES
      sources/module-internal-part.cxx)
