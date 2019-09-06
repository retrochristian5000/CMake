cmake_minimum_required(VERSION 3.15)
project(MocInclude)
include("../AutogenCoreTest.cmake")

#
# Tests moc file include patterns
#

# Include directories
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Generate .moc file externally and enabled SKIP_AUTOMOC on the file
qtx_generate_moc(
  ${CMAKE_CURRENT_SOURCE_DIR}/ExternDot.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/ExternDot.moc)
set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/ExternDot.cpp
             PROPERTY SKIP_AUTOMOC ON)

# Generate .moc file externally from generated source file
# and enabled SKIP_AUTOMOC on the source file
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.hpp
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/ExternDotGenerated.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.hpp)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.cpp
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/ExternDotGenerated.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.cpp)
qtx_generate_moc(
  ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.moc)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.cpp
             PROPERTY SKIP_AUTOMOC ON)

# Generate header moc file externally with a custom name
# and enabled SKIP_AUTOMOC on the header
qtx_generate_moc(
  ${CMAKE_CURRENT_SOURCE_DIR}/MixedCustom.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/MixedCustom_extMoc.cpp)
set_property(
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/MixedCustom.hpp
  PROPERTY SKIP_AUTOMOC ON)

# Custom target to depend on
set(SOBJC_MOC ${CMAKE_CURRENT_BINARY_DIR}/moc_MixedCustom.cpp)
add_custom_target("mocInclude_SOBJC"
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/MixedCustom_extMoc.cpp
  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/moc_MixedCustom.cpp
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/moc_MixedCustom.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/moc_MixedCustom.cpp)

# Executable
add_executable(mocInclude
  # Test own "*.moc" and "moc_*.cpp" includes
  None.cpp
  OwnDot.cpp
  OwnUnderscore.cpp
  OwnDotUnderscore.cpp

  # Test "moc_*.cpp" includes of other files
  OtherUnderscore.cpp
  OtherUnderscoreExtra.cpp
  OtherUnderscoreSub.cpp
  OtherUnderscoreSubDir/SubExtra.cpp

  # Test externally generated moc files
  ExternDot.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/ExternDot.moc

  # Test externally generated moc files for GENERATED source
  ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/ExternDotGenerated.moc

  # Test externally generated moc files and SKIP_AUTOMOC enabled header
  MixedSkipped.cpp
  MixedCustom.hpp
  MixedCustom.cpp

  # Test sources in a subdirectory
  SingleSub/SubOwnDot.cpp

  # Main source
  main.cpp
)
add_dependencies(mocInclude "mocInclude_SOBJC")
target_link_libraries(mocInclude ${QT_LIBRARIES})
set_target_properties(mocInclude PROPERTIES AUTOMOC ON)
