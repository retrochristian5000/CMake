find_package(Qt5Test REQUIRED)

include_directories(
  ${CMake_SOURCE_DIR}/Source
  ${CMake_SOURCE_DIR}/Source/QtDialog
  ${CMake_BINARY_DIR}/Source/QtDialog
  )

qt5_wrap_cpp(MOC_SRCS
  CMakeGUITest.h
  )
add_executable(CMakeGUITest CMakeGUITest.cxx ${MOC_SRCS})
target_link_libraries(CMakeGUITest CMakeGUIMainLib Qt5::Core Qt5::Test Qt5::Widgets)
target_compile_definitions(CMakeGUITest PRIVATE
  "CMake_SOURCE_DIR=\"${CMake_SOURCE_DIR}\""
  "CMake_BINARY_DIR=\"${CMake_BINARY_DIR}\""
  )

function(add_cmake_gui_test name)
  add_test(NAME "CMakeGUI.${name}" COMMAND CMakeGUITest ${ARGN})
  set_property(TEST "CMakeGUI.${name}" PROPERTY ENVIRONMENT "CMake_GUI_TEST_NAME=${name}")
endfunction()

file(RELATIVE_PATH _relative "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

add_cmake_gui_test(sourceAndBinaryDir
  -S "${CMAKE_CURRENT_SOURCE_DIR}/SimpleProject"
  -B "${CMAKE_CURRENT_BINARY_DIR}/sourceAndBinaryDir"
  )
add_cmake_gui_test(sourceAndBinaryDirRelative
  "-S${_relative}/SimpleProject"
  "-BsourceAndBinaryDirRelative"
  )
add_cmake_gui_test(sourceDir
  "${CMAKE_CURRENT_SOURCE_DIR}/SimpleProject"
  )
add_cmake_gui_test(sourceDirNoExist
  "${CMAKE_CURRENT_SOURCE_DIR}/NoExist"
  )
add_cmake_gui_test(binaryDir
  "${CMAKE_BINARY_DIR}"
  )
add_cmake_gui_test(binaryDirNoExist
  "${CMAKE_CURRENT_BINARY_DIR}/NoExist"
  )
