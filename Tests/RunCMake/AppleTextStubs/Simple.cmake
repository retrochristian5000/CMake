enable_language(C)

add_library(foo SHARED foo.c)
set_property(TARGET foo PROPERTY ENABLE_EXPORTS TRUE)

add_executable(main main.c)
target_link_libraries(main PRIVATE foo)


install(TARGETS foo DESTINATION "${CMAKE_BINARY_DIR}/INSTALL")

install(TARGETS foo LIBRARY DESTINATION "${CMAKE_BINARY_DIR}/INSTALL2/lib"
                    ARCHIVE DESTINATION "${CMAKE_BINARY_DIR}/INSTALL2/dev")


set (GENERATE_CONTENT [[
macro (CHECK_FILE test_msg path)
  if (NOT EXISTS "${path}")
    string (APPEND RunCMake_TEST_FAILED "${test_msg}: \"${path}\" not found\n")
  endif()
endmacro()

check_file("TBD file" "$<TARGET_IMPORT_FILE:foo>")
check_file("DYLIB file" "$<TARGET_FILE:foo>")
check_file("executable file" "$<TARGET_FILE:main>")

check_file("Installed TBD file" "${RunCMake_TEST_BINARY_DIR}/INSTALL/$<TARGET_IMPORT_FILE_NAME:foo>")
check_file("Installed TBD file" "${RunCMake_TEST_BINARY_DIR}/INSTALL2/dev/$<TARGET_IMPORT_FILE_NAME:foo>")
]])

file (GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Simple-$<CONFIG>-generated.cmake"
  CONTENT "${GENERATE_CONTENT}")
