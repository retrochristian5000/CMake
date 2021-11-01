
enable_language(C)

if(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
  set(CMAKE_LINK_USING_weak_library "PATH{-weak_library <LIBRARY>}NAME{LINKER:-weak-l<LIB_ITEM>}")
  set(CMAKE_LINK_USING_weak_library_SUPPORTED TRUE)
else()
  # feature not yet supported for the other environments
  set(CMAKE_LINK_USING_whole_library_SUPPORTED FALSE)
endif()

add_library(lib SHARED base.c lib.c unref.c)

add_executable(main main.c)
target_link_libraries(main PRIVATE "$<LINK_LIBRARY:weak_library,lib>")
