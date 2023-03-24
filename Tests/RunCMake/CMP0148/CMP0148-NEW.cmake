set(policyCommand "cmake_policy(SET CMP0148 NEW)")

set(projName top)
set(depName middle)
set(epRelativeGitRepo ../Deeper/Middle)
set(fcRelativeGitRepo ../Deeper/Middle)
configure_file(CMakeLists.txt.in Top/CMakeLists.txt @ONLY)
initGitRepo("${CMAKE_CURRENT_BINARY_DIR}/Top")

set(projName middle)
set(depName bottom)
set(epRelativeGitRepo ../../Bottom)
set(fcRelativeGitRepo ../../Bottom)
configure_file(CMakeLists.txt.in Deeper/Middle/CMakeLists.txt @ONLY)
initGitRepo("${CMAKE_CURRENT_BINARY_DIR}/Deeper/Middle")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Bottom")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Bottom/CMakeLists.txt" [[
cmake_minimum_required(VERSION 3.26)
project(bottom LANGUAGES NONE)
message(STATUS "Configured bottom project")
]])
initGitRepo("${CMAKE_CURRENT_BINARY_DIR}/Bottom")

set(clonedTopDir "${CMAKE_CURRENT_BINARY_DIR}/a/b")
file(MAKE_DIRECTORY "${clonedTopDir}")
execGitCommand(${CMAKE_CURRENT_BINARY_DIR} clone --quiet "file://${CMAKE_CURRENT_BINARY_DIR}/Top" "${clonedTopDir}")
add_subdirectory("${clonedTopDir}" "${CMAKE_CURRENT_BINARY_DIR}/cloned-build")

# Ensure build order is predictable
add_custom_target(non-ep-top ALL COMMAND ${CMAKE_COMMAND} -E echo "Non-ep top project")
add_dependencies(non-ep-top ep-middle)
add_dependencies(ep-middle ep-bottom)
