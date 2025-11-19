include(Setup.cmake)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_library(test INTERFACE)

install(
  TARGETS test
  EXPORT test_targets
)

export(
  EXPORT test_targets
  SBOM test_targets
  DESCRIPTION "An eloquent description"
  LICENSE "BSD-3"
  HOMEPAGE_URL "www.example.com"
  VERSION "1.3.4"
)
