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

if (check_step EQUAL 2)
  include(\"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/Makefile.cmake\")
  if (NOT CMAKE_DEPEND_INFO_FILES)
    set(RunCMake_TEST_FAILED \"Variable CMAKE_DEPEND_INFO_FILES not found.\")
  else()
    include(\"${CMAKE_CURRENT_BINARY_DIR}/\${CMAKE_DEPEND_INFO_FILES}\")
    if (NOT CMAKE_DEPENDS_DEPENDENCY_FILES)
      set(depend_make \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/main.dir/depend.make\")
      if (EXISTS \"\${depend_make}\")
        file(READ \"\${depend_make}\" depend_make_content)
        string(REPLACE \"\\\\\" \"\" depend_make_content \${depend_make_content})
        string(REGEX MATCHALL \"CMakeFiles/main.dir/main.c.o\" match_list \${depend_make_content})
        list(LENGTH match_list match_count)
        if (NOT match_count EQUAL 1)
          set(RunCMake_TEST_FAILED \"Depender main.c.o count '\${match_count}' in '\${depend_make}'.\")
        else()
          string(REGEX MATCHALL \"temp_header_file\" match_list \${depend_make_content})
          list(LENGTH match_list match_count)
          if (NOT match_count EQUAL 40000)
            set(RunCMake_TEST_FAILED \"Header dependees count '\${match_count}' in '\${depend_make}'.\")
          endif()
        endif()
      else()
        set(RunCMake_TEST_FAILED \"File '\${depend_make}' not found.\")
      endif()
    endif()
  endif()
endif()
")
