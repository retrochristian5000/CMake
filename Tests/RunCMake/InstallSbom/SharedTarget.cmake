include(Setup.cmake)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_library(shared SHARED main.c)

find_package(foo)
target_link_libraries(shared PUBLIC foo::foo)

install(
  TARGETS shared
  EXPORT shared_targets
  )

install(
  SBOM shared_targets
  EXPORT shared_targets
  DESTINATION .
  )
