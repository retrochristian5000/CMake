enable_language(C)

set(source "${CMAKE_CURRENT_BINARY_DIR}/source.c")
file(WRITE "${source}" "int example() { return 0; }\n")

# Simplification
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

add_library(shared SHARED "${source}")
add_library(static STATIC "${source}")
add_library(module MODULE "${source}")

function (add_check_command target expected)
  add_custom_command(
    OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/${target}-ok"
    DEPENDS "$<TARGET_FILE:${target}>"
            "${CMAKE_CURRENT_LIST_DIR}/CheckContent.cmake"
    COMMAND "${CMAKE_COMMAND}"
            # Forward information not available in script mode.
            "-DCMAKE_SHARED_LIBRARY_SUFFIX=${CMAKE_SHARED_LIBRARY_SUFFIX}"
            "-DCMAKE_SHARED_MODULE_SUFFIX=${CMAKE_SHARED_MODULE_SUFFIX}"
            "-DCMAKE_STATIC_LIBRARY_SUFFIX=${CMAKE_STATIC_LIBRARY_SUFFIX}"
            "-DCMAKE_IMPORT_LIBRARY_SUFFIX=${CMAKE_IMPORT_LIBRARY_SUFFIX}"
            # Forward required platform information.
            "-DWIN32=${WIN32}"
            "-DMINGW=${MINGW}"
            # Script variables.
            "-Dpath=$<TARGET_FILE:${target}>"
            "-Dexpected=${expected}"
            "-Dstamp=${CMAKE_CURRENT_BINARY_DIR}/${target}-ok"
            -P "${CMAKE_CURRENT_LIST_DIR}/CheckContent.cmake")
  add_custom_target("${target}-check" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${target}-ok")
endfunction ()

add_check_command(shared SHARED)
add_check_command(static STATIC)
add_check_command(module SHARED)
add_custom_target(check ALL DEPENDS shared-check static-check module-check)
