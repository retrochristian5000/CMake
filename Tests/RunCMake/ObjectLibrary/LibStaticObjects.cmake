
add_definitions(-DCOMPILE_FOR_SHARED_LIB)

add_library(StaticLib STATIC a.c)
set_target_properties(StaticLib PROPERTIES POSITION_INDEPENDENT_CODE ON)


add_library(UseStaticLib SHARED b.c $<TARGET_OBJECTS:StaticLib>)
target_compile_definitions(UseStaticLib PUBLIC REQUIRED)
set_target_properties(UseStaticLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(exe exe.c)
target_link_libraries(exe UseStaticLib)

add_custom_command(TARGET StaticLib POST_BUILD
  COMMAND  ${CMAKE_COMMAND} -E echo "OBJS \"$<TARGET_OBJECTS:StaticLib>\""
  )
