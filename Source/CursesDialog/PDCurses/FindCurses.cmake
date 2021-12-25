if(NOT WIN32)
  message(FATAL_ERROR "PDCurses not (yet) supported on non-Windows platforms")
endif()

file(GLOB PDCURSES_SOURCES "${CMAKE_CURRENT_LIST_DIR}/wincon/*.c" "${CMAKE_CURRENT_LIST_DIR}/pdcurses/*.c")
add_library(PDCurses ${PDCURSES_SOURCES})
target_include_directories(PDCurses PRIVATE "${CMAKE_CURRENT_LIST_DIR}")

set(CURSES_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}")
set(CURSES_LIBRARY PDCurses)
set(CURSES_FOUND ON)
