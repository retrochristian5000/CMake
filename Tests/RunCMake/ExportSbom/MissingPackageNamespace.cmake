include(Setup.cmake)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_executable(test
  main.c
)

find_package(
  baz REQUIRED
  NO_DEFAULT_PATH
  PATHS ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(test PUBLIC baz)

install(
  TARGETS test
  EXPORT test_targets
)

export(EXPORT test_targets
  SBOM test_targets
  VERSION "1.0.2"
)
