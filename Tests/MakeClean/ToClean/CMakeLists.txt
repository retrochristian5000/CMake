cmake_minimum_required(VERSION 3.14)
project(ToClean)

# Utility variables
set(TSD ${ToClean_SOURCE_DIR})
set(TBD ${ToClean_BINARY_DIR})
if("${CMAKE_GENERATOR}" MATCHES "Makefile")
  set(_ADD_GEN_CLEAN_FILES ADDITIONAL_MAKE_CLEAN_FILES)
elseif("${CMAKE_GENERATOR}" MATCHES "Ninja")
  set(_ADD_GEN_CLEAN_FILES ADDITIONAL_NINJA_CLEAN_FILES)
endif()

# Lists build-time-generated files that should be cleaned away
set(TOCLEAN_FILES)

# Build a simple project whose compiled objects should be cleaned.
add_executable(toclean toclean.cxx)
list(APPEND TOCLEAN_FILES
  "${TBD}${CMAKE_FILES_DIRECTORY}/toclean.dir/toclean.cxx${CMAKE_CXX_OUTPUT_EXTENSION}")

# Create a file that must be registered for cleaning.
file(WRITE "${TBD}/Registered.txt" "File registered for cleaning.\n")
set_directory_properties(PROPERTIES
  ${_ADD_GEN_CLEAN_FILES} "${TBD}/Registered.txt")
list(APPEND TOCLEAN_FILES "${TBD}/Registered.txt")

# Create a custom command whose output should be cleaned.
add_custom_command(OUTPUT ${TBD}/generated.txt
  DEPENDS ${TSD}/toclean.cxx
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy ${TSD}/toclean.cxx ${TBD}/generated.txt
  )
add_custom_target(generate ALL DEPENDS ${TBD}/generated.txt)
list(APPEND TOCLEAN_FILES "${TBD}/generated.txt")

# Create a custom command whose output should be cleaned, but whose name
# is not known until generate-time
set(copied_exe "$<TARGET_FILE_DIR:toclean>/toclean_copy${CMAKE_EXECUTABLE_SUFFIX}")
add_custom_command(TARGET toclean POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy $<TARGET_FILE:toclean> ${copied_exe}
  )
set_property(DIRECTORY APPEND PROPERTY
  ${_ADD_GEN_CLEAN_FILES} ${copied_exe})
list(APPEND TOCLEAN_FILES "${TBD}/toclean_copy${CMAKE_EXECUTABLE_SUFFIX}")

# Configure a file listing these build-time-generated files.
configure_file(${TSD}/ToCleanFiles.cmake.in ${TBD}/ToCleanFiles.cmake @ONLY)
