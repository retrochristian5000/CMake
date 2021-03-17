include(${CMAKE_CURRENT_LIST_DIR}/../macros.cmake)

ASSERT_FILES_EQUAL(
  ${CMAKE_CURRENT_BINARY_DIR}/TestLog.txt
  ${CMAKE_CURRENT_LIST_DIR}/ExpectedLog.txt
)