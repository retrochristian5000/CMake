add_library(SharedLib SHARED a.c)
target_compile_definitions(SharedLib PRIVATE COMPILE_FOR_SHARED_LIB)


add_library(UseSharedLib SHARED b.c $<TARGET_OBJECTS:SharedLib>)
target_compile_definitions(UseSharedLib PUBLIC REQUIRED)
target_compile_definitions(UseSharedLib PRIVATE COMPILE_FOR_SHARED_LIB)

add_executable(exe exe.c)
target_link_libraries(exe UseSharedLib)

#Test static library objects with file(GENERATE)
file(GENERATE
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gen_$<LOWER_CASE:$<CONFIG>/>path_to_objs.h
     CONTENT [[

#include <vector>
#include <string>

#ifndef path_to_objs
#define path_to_objs

static std::string obj_paths = "$<TARGET_OBJECTS:SharedLib>";

#endif

]]
)

add_custom_command(TARGET SharedLib POST_BUILD
  COMMAND  ${CMAKE_COMMAND} -E echo "OBJS \"$<TARGET_OBJECTS:SharedLib>\""
  )
