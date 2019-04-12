add_executable(ExecObjs a.c b.c exe.c)
target_compile_definitions(ExecObjs PRIVATE REQUIRED)

add_executable(UseExecObjs $<TARGET_OBJECTS:ExecObjs>)
target_compile_definitions(UseExecObjs PRIVATE REQUIRED)

add_custom_command(TARGET ExecObjs POST_BUILD
  COMMAND  ${CMAKE_COMMAND} -E echo "OBJS \"$<TARGET_OBJECTS:ExecObjs>\""
  )
