add_executable(ExecObjs a.c b.c exe.c)
target_compile_definitions(ExecObjs PRIVATE REQUIRED)

add_executable(UseExecObjs $<TARGET_OBJECTS:ExecObjs>)
target_compile_definitions(UseExecObjs PRIVATE REQUIRED)

#Test static library objects with file(GENERATE)
file(GENERATE
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gen_$<LOWER_CASE:$<CONFIG>/>path_to_objs.h
     CONTENT [[

#include <vector>
#include <string>

#ifndef path_to_objs
#define path_to_objs

static std::string obj_paths = "$<TARGET_OBJECTS:ExecObjs>";

#endif

]]
)

add_custom_command(TARGET ExecObjs POST_BUILD
  COMMAND  ${CMAKE_COMMAND} -E echo "OBJS \"$<TARGET_OBJECTS:ExecObjs>\""
  )
