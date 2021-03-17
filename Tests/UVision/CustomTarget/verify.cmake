include(${CMAKE_CURRENT_LIST_DIR}/../macros.cmake)

ASSERT_FILE_CONTENT_MATCHES(
  ${CMAKE_CURRENT_BINARY_DIR}/TestLog.txt
  "Built CustomTarget"
)