enable_language(CXX)

add_library(export-modules)
target_sources(export-modules
  PUBLIC
    FILE_SET fs TYPE CXX_MODULES FILES
      sources/module.cxx)
set_property(TARGET export-modules
  PROPERTY EXPORT_NAME export-name)

install(TARGETS export-modules
  EXPORT exp
  FILE_SET fs DESTINATION "include/cxx/export-modules")

install(EXPORT exp
  DESTINATION "lib/cmake/export-modules"
  CXX_MODULES_DIRECTORY "cxx-modules")
