add_library(SharedLib SHARED a.c)
target_compile_definitions(SharedLib PRIVATE COMPILE_FOR_SHARED_LIB)


add_library(UseSharedLib SHARED b.c $<TARGET_OBJECTS:SharedLib>)
target_compile_definitions(UseSharedLib PUBLIC REQUIRED)
target_compile_definitions(UseSharedLib PRIVATE COMPILE_FOR_SHARED_LIB)

add_executable(exe exe.c)
target_link_libraries(exe UseSharedLib)

add_custom_command(TARGET SharedLib POST_BUILD
  COMMAND  ${CMAKE_COMMAND} -E echo "OBJS \"$<TARGET_OBJECTS:SharedLib>\""
  )
