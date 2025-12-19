enable_language(CXX)

add_executable(generate_binary generate_binary.cpp)

include("${CMAKE_CURRENT_SOURCE_DIR}/hashes.cmake")

function(generate_binary name)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
    COMMAND generate_binary "${name}" "${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
    DEPENDS generate_binary
    )
  add_custom_target(
    verify_${name} ALL
    COMMAND "${CMAKE_COMMAND}"
      "-DFILENAME=${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
      "-DSHA256SUM=${${name}_hash}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_binary.cmake"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
    )
endfunction()

foreach(name IN ITEMS basic empty text_lf text_crlf text_align long)
  generate_binary("${name}")
endforeach()

include("${CMAKE_CURRENT_SOURCE_DIR}/jack_count.cmake")

if(CMake_TEST_BIN2C_LARGE_FILE)
  add_executable(generate_very_long generate_very_long.cpp)
  target_compile_definitions(generate_very_long PRIVATE JACK_COUNT=${jack_count})
  add_custom_target(
    verify_very_long_hash
    COMMAND "${CMAKE_COMMAND}"
      "-DGENERATE_VERY_LONG=$<TARGET_FILE:generate_very_long>"
      "-DSHA256SUM=${very_long_hash}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_very_long_hash.cmake"
    )

  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/very_long_executables.cmake"
    COMMAND "${CMAKE_COMMAND}"
      "-DGENERATE_VERY_LONG=$<TARGET_FILE:generate_very_long>"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/record_very_long.cmake"
    )
  add_custom_target(
    record_very_long ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/very_long_executables.cmake"
    )
endif()
