include(Setup.cmake)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_executable(application
  main.c
)

find_package(
  bar 1.3.4 REQUIRED
  NO_DEFAULT_PATH
  PATHS ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(application PUBLIC bar::bar)

install(
  TARGETS application
  EXPORT application_targets
)

export(EXPORT application_targets
  SBOM application_targets
)
