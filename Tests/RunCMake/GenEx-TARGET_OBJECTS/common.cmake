function(check_target_objects name genex)
  add_custom_target(${name}_check ALL
    COMMAND ${CMAKE_COMMAND}
      "-DEXPECTED=${ARGN}"
      "-DACTUAL=${genex}"
      -P "${CMAKE_SOURCE_DIR}/check_target_objects.cmake"
    VERBATIM
    )
endfunction()
