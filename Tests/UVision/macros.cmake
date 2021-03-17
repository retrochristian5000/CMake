macro(ASSERT_FILES_EQUAL file1 file2)
  execute_process(
    COMMAND ${CMAKE_COMMAND}
    -E compare_files --ignore-eol ${file1} ${file2}
    RESULT_VARIABLE result
  )
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "Files not equal: ${file1} ${file2}")
  endif()
endmacro()

macro(ASSERT_FILE_CONTENT_MATCHES file)
  if(${ARGC} GREATER 1)
    file(READ ${file} file_content)
    foreach(regex ${ARGN})
    if (NOT file_content MATCHES ${regex})
      message(FATAL_ERROR "Content of file does not match regex: \"${regex}\" ${file}")
    endif()
    endforeach()
  endif()
endmacro()

macro(ASSERT_FILE_EXISTS file)
  if (NOT EXISTS file)
    message(FATAL_ERROR "Expected file does not exist: ${file}")
  endif()
endmacro()