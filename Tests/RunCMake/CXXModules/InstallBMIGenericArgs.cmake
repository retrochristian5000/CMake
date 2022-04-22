enable_language(CXX)

add_library(install-bmi-generic-args)
target_sources(install-bmi-generic-args
  PUBLIC
    FILE_SET public TYPE CXX_MODULES FILES
      sources/module.cxx)

install(TARGETS install-bmi-generic-args
  DESTINATION "bin")
