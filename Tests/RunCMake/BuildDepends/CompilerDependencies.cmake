enable_language(C)

add_executable(main ${CMAKE_CURRENT_BINARY_DIR}/main.c)

file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/check-$<LOWER_CASE:$<CONFIG>>.cmake CONTENT "
set(check_pairs
  \"$<TARGET_FILE:main>|${CMAKE_CURRENT_BINARY_DIR}/main.c\"
  \"$<TARGET_FILE:main>|${CMAKE_CURRENT_BINARY_DIR}/main.h\"
  )
set(check_exes
  \"$<TARGET_FILE:main>\"
  )

include(\"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/Makefile.cmake\")
if (NOT CMAKE_DEPEND_INFO_FILES)
  set(RunCMake_TEST_FAILED \"Variable CMAKE_DEPEND_INFO_FILES not found.\")
else()
  include(\"${CMAKE_CURRENT_BINARY_DIR}/\${CMAKE_DEPEND_INFO_FILES}\")
  if (NOT CMAKE_DEPENDS_DEPENDENCY_FILES)
    set(RunCMake_TEST_FAILED \"Variable CMAKE_DEPENDS_DEPENDENCY_FILES not found.\")
  else()
    cmake_path(REMOVE_EXTENSION CMAKE_DEPENDS_DEPENDENCY_FILES LAST_ONLY
      OUTPUT_VARIABLE OBJECT_FILE)
    file(READ \"${CMAKE_CURRENT_BINARY_DIR}/\${CMAKE_DEPENDS_DEPENDENCY_FILES}\" DEPENDS_CONTENT)
    string(REPLACE \"\\\\ \" \" \" DEPENDS_CONTENT \"\${DEPENDS_CONTENT}\")
    if(NOT DEPENDS_CONTENT MATCHES \"\${OBJECT_FILE} *:.+${CMAKE_CURRENT_BINARY_DIR}/main.c\"
         OR NOT DEPENDS_CONTENT MATCHES \"${CMAKE_CURRENT_BINARY_DIR}/main.h\")
      set(RunCMake_TEST_FAILED \"Dependency file '${CMAKE_CURRENT_BINARY_DIR}/\${CMAKE_DEPENDS_DEPENDENCY_FILES}' badly generated.\")
    endif()
  endif()
endif()
")
