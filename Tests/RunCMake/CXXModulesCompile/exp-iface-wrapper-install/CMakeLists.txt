cmake_minimum_required(VERSION 3.24...3.28)
project(cxx_modules_export_interface_wrapper CXX)

include("${CMAKE_SOURCE_DIR}/../cxx-modules-rules.cmake")

add_library(export_wrapped_modules STATIC)
target_sources(export_wrapped_modules
  PRIVATE
    forward.cxx
  PRIVATE
    FILE_SET modules_private TYPE CXX_MODULES
      BASE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}"
      FILES
        private.cxx
  PUBLIC
    FILE_SET modules TYPE CXX_MODULES
      BASE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}"
      FILES
        importable.cxx
        subdir/importable.cxx
  )
target_compile_features(export_wrapped_modules PUBLIC cxx_std_20)

# An interface library without modules that forwards to the module provider.
add_library(wrapper INTERFACE)
target_link_libraries(wrapper INTERFACE export_wrapped_modules)

install(TARGETS export_wrapped_modules wrapper
  EXPORT CXXModules
  FILE_SET modules DESTINATION "lib/cxx/miu")
install(EXPORT CXXModules
  NAMESPACE CXXModules::
  DESTINATION "lib/cmake/export_interface_wrapper"
  FILE "export_interface_wrapper-targets.cmake"
  CXX_MODULES_DIRECTORY "export_interface_wrapper-cxx-modules")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/export_interface_wrapper-config.cmake"
  "include(\"\${CMAKE_CURRENT_LIST_DIR}/export_interface_wrapper-targets.cmake\")
set(\${CMAKE_FIND_PACKAGE_NAME}_FOUND 1)
")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/export_interface_wrapper-config.cmake"
  DESTINATION "lib/cmake/export_interface_wrapper")

set(generator
  -G "${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR_TOOLSET)
  list(APPEND generator
    -T "${CMAKE_GENERATOR_TOOLSET}")
endif ()
if (CMAKE_GENERATOR_PLATFORM)
  list(APPEND generator
    -A "${CMAKE_GENERATOR_PLATFORM}")
endif ()

add_test(NAME export_interface_wrapper_build
  COMMAND
    "${CMAKE_COMMAND}"
    "-Dexport_interface_wrapper_DIR=${CMAKE_INSTALL_PREFIX}/lib/cmake/export_interface_wrapper"
    ${generator}
    -S "${CMAKE_CURRENT_SOURCE_DIR}/test"
    -B "${CMAKE_CURRENT_BINARY_DIR}/test")
