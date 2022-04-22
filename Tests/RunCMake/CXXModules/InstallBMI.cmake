enable_language(CXX)

add_library(install-bmi)
target_sources(install-bmi
  PUBLIC
    FILE_SET public TYPE CXX_MODULES FILES
      sources/module.cxx)

install(TARGETS install-bmi
  CXX_MODULES_BMI
    DESTINATION "lib/bmi"
    COMPONENT "bmi")

install(TARGETS install-bmi
  CXX_MODULES_BMI
    DESTINATION "lib/bmi"
    EXCLUDE_FROM_ALL
    COMPONENT "bmi-optional")

install(TARGETS install-bmi
  CXX_MODULES_BMI
    DESTINATION "lib/bmi"
    CONFIGURATIONS Debug
    COMPONENT "bmi-only-debug")
